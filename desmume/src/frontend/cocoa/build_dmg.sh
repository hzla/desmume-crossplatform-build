#   Copyright 2006 Guillaume Duhamel
#   Copyright 2006 Lawrence Sebald
#   Copyright 2007 Anders Montonen
#
#   This file is part of Yabause.
#
#   Yabause is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   Yabause is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with Yabause; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA


#!/bin/bash

set -euo pipefail

script_dir="$(cd "$(dirname "$0")" && pwd)"
workspace_root="$(cd "$script_dir/../../../.." && pwd)"
source_root="$(cd "$script_dir/../../.." && pwd)"
release_dir="$workspace_root/releases"
staging_dir="$(mktemp -d "${TMPDIR:-/tmp}/desmume-dmg.XXXXXX")"
app_path=""
final_dmg=""
requested_flavor="auto"

usage() {
	cat <<EOF
Usage: $(basename "$0") [--app-path PATH] [--output PATH] [--flavor auto|intel|apple-silicon|universal]

Creates a DMG from the specified DeSmuME.app bundle. If --app-path is omitted,
the script prefers build/Merged Release/DeSmuME.app and then build/Release/DeSmuME.app.
EOF
}

resolve_app_path() {
	local merged_app="$workspace_root/build/Merged Release/DeSmuME.app"
	local release_app="$workspace_root/build/Release/DeSmuME.app"

	if [ -d "$merged_app" ]; then
		printf '%s\n' "$merged_app"
		return 0
	fi

	if [ -d "$release_app" ]; then
		printf '%s\n' "$release_app"
		return 0
	fi

	return 1
}

ensure_supported_release_architecture() {
	local app_path="$1"
	local required_flavor="$2"
	local binary_path="$app_path/Contents/MacOS/DeSmuME"

	if [ ! -f "$binary_path" ]; then
		echo "Missing app binary at $binary_path" >&2
		exit 1
	fi

	local archs
	archs="$(lipo -archs "$binary_path" 2>/dev/null || true)"
	if [ -z "$archs" ]; then
		echo "Unable to determine app architectures for $binary_path" >&2
		exit 1
	fi

	local detected_flavor=""
	local has_arm64="false"
	local has_intel="false"

	case " $archs " in
		*" arm64 "*) has_arm64="true" ;;
	esac

	case " $archs " in
		*" x86_64 "*|*" i386 "*|*" ppc "*) has_intel="true" ;;
	esac

	if [ "$has_arm64" = "true" ] && [ "$has_intel" = "true" ]; then
		detected_flavor="universal"
	elif [ "$has_arm64" = "true" ]; then
		detected_flavor="apple-silicon"
	elif [ "$has_intel" = "true" ]; then
		detected_flavor="intel"
	else
		echo "Refusing to package $binary_path because it has unsupported architectures: $archs" >&2
		exit 1
	fi

	case "$required_flavor" in
		auto) ;;
		universal)
			if [ "$detected_flavor" != "universal" ]; then
				echo "Expected a universal app at $binary_path, but found $detected_flavor ($archs)." >&2
				exit 1
			fi
			;;
		intel)
			if [ "$detected_flavor" != "intel" ] && [ "$detected_flavor" != "universal" ]; then
				echo "Expected an Intel-compatible app at $binary_path, but found $detected_flavor ($archs)." >&2
				exit 1
			fi
			;;
		apple-silicon)
			if [ "$detected_flavor" != "apple-silicon" ] && [ "$detected_flavor" != "universal" ]; then
				echo "Expected an Apple Silicon-compatible app at $binary_path, but found $detected_flavor ($archs)." >&2
				exit 1
			fi
			;;
		*)
			echo "Unknown DMG flavor: $required_flavor" >&2
			exit 1
			;;
	esac

	printf '%s\n' "$detected_flavor"
}

default_output_name_for_flavor() {
	local flavor="$1"

	case "$flavor" in
		universal) printf '%s\n' "DeSmuME.dmg" ;;
		intel) printf '%s\n' "DeSmuME-intel.dmg" ;;
		apple-silicon) printf '%s\n' "DeSmuME-apple-silicon.dmg" ;;
		*)
			echo "Unknown DMG flavor: $flavor" >&2
			exit 1
			;;
	esac
}

cleanup() {
	rm -rf "$staging_dir"
}

trap cleanup EXIT

while [ "$#" -gt 0 ]; do
	case "$1" in
		--app-path)
			if [ "$#" -lt 2 ]; then
				echo "--app-path requires a value" >&2
				exit 1
			fi
			app_path="$2"
			shift 2
			;;
		--output)
			if [ "$#" -lt 2 ]; then
				echo "--output requires a value" >&2
				exit 1
			fi
			final_dmg="$2"
			shift 2
			;;
		--flavor)
			if [ "$#" -lt 2 ]; then
				echo "--flavor requires a value" >&2
				exit 1
			fi
			requested_flavor="$2"
			shift 2
			;;
		-h|--help)
			usage
			exit 0
			;;
		*)
			echo "Unknown argument: $1" >&2
			usage >&2
			exit 1
			;;
	esac
done

if [ -z "$app_path" ]; then
	app_path="$(resolve_app_path || true)"
fi

if [ ! -d "$app_path" ]; then
	echo "Missing app bundle: $app_path" >&2
	exit 1
fi

resolved_flavor="$(ensure_supported_release_architecture "$app_path" "$requested_flavor")"

if [ -z "$final_dmg" ]; then
	final_dmg="$release_dir/$(default_output_name_for_flavor "$resolved_flavor")"
fi

mkdir -p "$(dirname "$final_dmg")"
cp -R "$app_path" "$staging_dir/"
cp "$source_root/ChangeLog" "$staging_dir/"
cp "$source_root/README" "$staging_dir/"
cp "$source_root/README.MAC" "$staging_dir/"
cp "$source_root/AUTHORS" "$staging_dir/"
cp "$source_root/COPYING" "$staging_dir/"

hdiutil create -fs HFS+ -srcfolder "$staging_dir" -volname DeSmuME -format UDZO -ov "$final_dmg"
