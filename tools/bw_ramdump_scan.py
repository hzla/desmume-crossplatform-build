#!/usr/bin/env python3

import argparse
from pathlib import Path


ARM9_RAM_BASE = 0x02000000
ARM9_RAM_END = 0x02400000

GEN5_PARTY_MON_SIZE = 0xDC
GEN5_BOX_MON_SIZE = 0x88
GEN5_CORE_BYTES = 0x80

LCRNG_MULT = 0x41C64E6D
LCRNG_ADD = 0x6073

SHUFFLE_TABLE = (
    (0x08, 0x28, 0x48, 0x68), (0x08, 0x28, 0x68, 0x48), (0x08, 0x48, 0x28, 0x68), (0x08, 0x68, 0x28, 0x48),
    (0x08, 0x48, 0x68, 0x28), (0x08, 0x68, 0x48, 0x28), (0x28, 0x08, 0x48, 0x68), (0x28, 0x08, 0x68, 0x48),
    (0x48, 0x08, 0x28, 0x68), (0x68, 0x08, 0x28, 0x48), (0x48, 0x08, 0x68, 0x28), (0x68, 0x08, 0x48, 0x28),
    (0x28, 0x48, 0x08, 0x68), (0x28, 0x68, 0x08, 0x48), (0x48, 0x28, 0x08, 0x68), (0x68, 0x28, 0x08, 0x48),
    (0x48, 0x68, 0x08, 0x28), (0x68, 0x48, 0x08, 0x28), (0x28, 0x48, 0x68, 0x08), (0x28, 0x68, 0x48, 0x08),
    (0x48, 0x28, 0x68, 0x08), (0x68, 0x28, 0x48, 0x08), (0x48, 0x68, 0x28, 0x08), (0x68, 0x48, 0x28, 0x08),
)


BW_BASE_ROOTS = {
    "D": {"language": "GER", "enemy": 0x226AC34, "cgear_enemy": 0x225CE50},
    "F": {"language": "FRE", "enemy": 0x226AC74, "cgear_enemy": 0x225CE90},
    "I": {"language": "ITA", "enemy": 0x226ABF4, "cgear_enemy": 0x225CE10},
    "J": {"language": "JPN", "enemy": 0x226AB54, "cgear_enemy": 0x225CC84},
    "K": {"language": "KOR", "enemy": 0x226B3F4, "cgear_enemy": 0x225D610},
    "O": {"language": "USA", "enemy": 0x226ACF4, "cgear_enemy": 0x225CF10},
    "S": {"language": "SPA", "enemy": 0x226ACB4, "cgear_enemy": 0x225CED0},
}


def read_u16(data, offset):
    return data[offset] | (data[offset + 1] << 8)


def read_u32(data, offset):
    return (
        data[offset]
        | (data[offset + 1] << 8)
        | (data[offset + 2] << 16)
        | (data[offset + 3] << 24)
    )


def advance_lcrng(seed):
    return (LCRNG_MULT * seed + LCRNG_ADD) & 0xFFFFFFFF


def format_addr(offset):
    return f"0x{ARM9_RAM_BASE + offset:08X}"


def offset_from_ram_addr(ram_addr):
    offset = ram_addr - ARM9_RAM_BASE
    if offset < 0:
        return None
    return offset


def parse_bw_runtime_roots(game_code):
    if len(game_code) != 4:
        raise ValueError("game code must be 4 characters")

    version_code = game_code[2]
    language_code = game_code[3]
    if version_code not in ("A", "B"):
        raise ValueError(f"{game_code} is not a BW game code")
    if language_code not in BW_BASE_ROOTS:
        raise ValueError(f"unsupported BW language code {language_code}")

    base = BW_BASE_ROOTS[language_code]
    white_delta = 0x20 if (version_code == "A" and language_code != "K") else 0
    return {
        "language": base["language"],
        "enemy": base["enemy"] + white_delta,
        "cgear_enemy": base["cgear_enemy"] + white_delta,
    }


def decrypt_gen5_core(data, offset):
    pid = read_u32(data, offset)
    checksum = read_u16(data, offset + 0x6)
    seed = checksum
    checksum_sum = 0
    decrypted_words = [0] * (GEN5_CORE_BYTES // 2)

    core_offset = offset + 0x8
    for i in range(GEN5_CORE_BYTES // 2):
        seed = advance_lcrng(seed)
        raw_word = read_u16(data, core_offset + (i * 2))
        word = raw_word ^ (seed >> 16)
        decrypted_words[i] = word
        checksum_sum = (checksum_sum + word) & 0xFFFF

    return pid, checksum, checksum_sum == checksum, decrypted_words


def decrypt_gen5_tail_party(data, offset, pid):
    tail_offset = offset + 0x8 + GEN5_CORE_BYTES
    tail_word_count = (GEN5_PARTY_MON_SIZE - (0x8 + GEN5_CORE_BYTES)) // 2
    seed = pid
    decrypted = [0] * tail_word_count

    for i in range(tail_word_count):
        seed = advance_lcrng(seed)
        raw_word = read_u16(data, tail_offset + (i * 2))
        decrypted[i] = raw_word ^ (seed >> 16)

    return decrypted


def core_word_at(decrypted_words, byte_offset):
    return decrypted_words[(byte_offset - 0x8) // 2]


def parse_gen5_mon_at(data, offset, party=True):
    size = GEN5_PARTY_MON_SIZE if party else GEN5_BOX_MON_SIZE
    if offset < 0 or offset + size > len(data):
        return None

    pid, checksum, checksum_valid, decrypted_words = decrypt_gen5_core(data, offset)
    if not checksum_valid:
        return None

    shuffle_index = ((pid & 0x3E000) >> 13) % 24
    block_a, block_b, block_c, block_d = SHUFFLE_TABLE[shuffle_index]
    species = core_word_at(decrypted_words, block_a + 0x0)
    held_item = core_word_at(decrypted_words, block_a + 0x2)
    experience = core_word_at(decrypted_words, block_a + 0x8) | (core_word_at(decrypted_words, block_a + 0xA) << 16)
    ability_word = core_word_at(decrypted_words, block_a + 0xC)
    ability = (ability_word >> 8) & 0xFF
    moves = tuple(core_word_at(decrypted_words, block_b + (i * 2)) for i in range(4))

    result = {
        "pid": pid,
        "checksum": checksum,
        "species": species,
        "held_item": held_item,
        "experience": experience,
        "ability": ability,
        "moves": moves,
        "offset": offset,
        "party": party,
        "shuffle_index": shuffle_index,
        "block_a": block_a,
        "block_b": block_b,
        "block_c": block_c,
        "block_d": block_d,
    }

    if party:
        tail = decrypt_gen5_tail_party(data, offset, pid)
        status_raw = tail[0] | (tail[1] << 16)
        level = tail[2] & 0xFF
        current_hp = tail[3]
        max_hp = tail[4]
        result.update(
            {
                "status_raw": status_raw,
                "level": level,
                "current_hp": current_hp,
                "max_hp": max_hp,
            }
        )

    return result


def is_reasonable_mon(mon):
    if mon is None:
        return False
    if not (1 <= mon["species"] <= 649):
        return False
    if any(move < 0 or move > 700 for move in mon["moves"]):
        return False
    if mon["ability"] > 255:
        return False
    if mon["party"]:
        if mon["level"] < 1 or mon["level"] > 100:
            return False
        if mon["max_hp"] < 1 or mon["max_hp"] > 999:
            return False
        if mon["current_hp"] < 0 or mon["current_hp"] > mon["max_hp"]:
            return False
    return True


def print_mon(label, mon):
    if mon is None:
        print(f"{label}: <invalid>")
        return

    hp_text = ""
    if mon["party"]:
        hp_text = f" level={mon['level']} hp={mon['current_hp']}/{mon['max_hp']}"
    print(
        f"{label}: addr={format_addr(mon['offset'])} species={mon['species']} "
        f"moves={list(mon['moves'])} ability={mon['ability']} pid=0x{mon['pid']:08X}{hp_text}"
    )


def dump_known_enemy_slots(data, game_code):
    roots = parse_bw_runtime_roots(game_code)
    print(f"Known BW roots for {game_code}: language={roots['language']} enemy={roots['enemy']:#010x} cgear_enemy={roots['cgear_enemy']:#010x}")

    enemy_base = offset_from_ram_addr(roots["enemy"])
    cgear_base = offset_from_ram_addr(roots["cgear_enemy"])

    print("\nEnemy party-like slots:")
    for slot in range(6):
        mon = parse_gen5_mon_at(data, enemy_base + (slot * GEN5_PARTY_MON_SIZE), party=True)
        if is_reasonable_mon(mon):
            print_mon(f"  slot {slot + 1}", mon)
        else:
            print(f"  slot {slot + 1}: <invalid>")

    cgear_mon = parse_gen5_mon_at(data, cgear_base, party=True)
    print("\nC-Gear enemy mon:")
    if is_reasonable_mon(cgear_mon):
        print_mon("  cgear", cgear_mon)
    else:
        print("  cgear: <invalid>")


def parse_match(text):
    species_text, moves_text = text.split(":", 1)
    moves = tuple(int(part, 10) for part in moves_text.split(","))
    if len(moves) != 4:
        raise ValueError("match moves must contain exactly 4 comma-separated ids")
    return int(species_text, 10), moves


def full_scan(data, matches):
    print("\nFull every-byte scan:")
    wanted = {species: moves for species, moves in matches}
    found = []
    limit = len(data) - GEN5_PARTY_MON_SIZE

    for offset in range(limit + 1):
        pid, checksum, checksum_valid, decrypted_words = decrypt_gen5_core(data, offset)
        if not checksum_valid:
            continue

        shuffle_index = ((pid & 0x3E000) >> 13) % 24
        block_a, block_b, _, _ = SHUFFLE_TABLE[shuffle_index]
        species = core_word_at(decrypted_words, block_a + 0x0)
        wanted_moves = wanted.get(species)
        if wanted_moves is None:
            continue

        moves = tuple(core_word_at(decrypted_words, block_b + (i * 2)) for i in range(4))
        if moves != wanted_moves:
            continue

        mon = parse_gen5_mon_at(data, offset, party=True)
        if not is_reasonable_mon(mon):
            continue
        found.append(mon)

    if not found:
        print("  no matches found")
        return

    print(f"  found {len(found)} matches")
    for mon in found:
        print_mon("  match", mon)


def main():
    parser = argparse.ArgumentParser(description="Scan a BW ARM9 RAM dump for Gen 5 Pokemon blobs.")
    parser.add_argument("dump_path", help="Path to the 4 MiB ARM9 RAM dump")
    parser.add_argument("--game-code", default="IRAO", help="Loaded BW game code (default: IRAO)")
    parser.add_argument(
        "--match",
        action="append",
        default=[],
        help="Species and moves to match, e.g. 302:261,105,282,325",
    )
    args = parser.parse_args()

    dump_path = Path(args.dump_path)
    data = dump_path.read_bytes()
    if len(data) != (ARM9_RAM_END - ARM9_RAM_BASE):
        print(f"warning: dump size is {len(data)} bytes, expected {ARM9_RAM_END - ARM9_RAM_BASE}")

    dump_known_enemy_slots(data, args.game_code)

    matches = [parse_match(value) for value in args.match]
    if matches:
        full_scan(data, matches)


if __name__ == "__main__":
    main()
