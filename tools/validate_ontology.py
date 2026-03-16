#!/usr/bin/env python3
"""
tools/validate_ontology.py

Validates the structure of data/ontology.json before it is bootstrapped
into the CBOR brain. Catches broken configs at commit time rather than
at runtime where corruption is harder to diagnose.

Usage:
    python3 tools/validate_ontology.py data/ontology.json

Exit codes:
    0  — valid
    1  — validation errors found
"""

import json
import sys


VALID_INTERACTION_TYPES = {
    "co-production",
    "inter-action",
    "feed-back",
    "dissipation",
}

REQUIRED_SEED_FIELDS = ["name", "cardinality", "quality"]
REQUIRED_INTERACTION_FIELDS = ["type", "input_A", "input_B", "output", "verb"]


def validate(path: str) -> list[str]:
    errors = []

    try:
        with open(path) as f:
            o = json.load(f)
    except FileNotFoundError:
        return [f"file not found: {path}"]
    except json.JSONDecodeError as e:
        return [f"invalid JSON: {e}"]

    seeds = {}

    # --- Validate seeds ---
    for idx, s in enumerate(o.get("seeds", [])):
        name = s.get("name", f"<seed[{idx}]>")
        for field in REQUIRED_SEED_FIELDS:
            if field not in s:
                errors.append(f"seed '{name}' missing required field '{field}'")
        if "cardinality" in s and not isinstance(s["cardinality"], (int, float)):
            errors.append(f"seed '{name}': 'cardinality' must be a number")
        if "quality" in s and not isinstance(s["quality"], (int, float)):
            errors.append(f"seed '{name}': 'quality' must be a number")
        if "aliases" in s and not isinstance(s["aliases"], list):
            errors.append(f"seed '{name}': 'aliases' must be an array")
        seeds[name] = s

    # --- Validate interactions ---
    for idx, i in enumerate(o.get("interactions", [])):
        label = f"interaction[{idx}] (output: '{i.get('output', '?')}')"

        for field in REQUIRED_INTERACTION_FIELDS:
            if field not in i:
                errors.append(f"{label} missing required field '{field}'")

        itype = i.get("type")
        if itype and itype not in VALID_INTERACTION_TYPES:
            errors.append(
                f"{label} has invalid type '{itype}'. "
                f"Must be one of: {sorted(VALID_INTERACTION_TYPES)}"
            )

        input_a = i.get("input_A")
        input_b = i.get("input_B")

        if input_a and input_a not in seeds:
            errors.append(
                f"{label}: input_A '{input_a}' is not defined as a seed"
            )
        if input_b and input_b not in seeds:
            errors.append(
                f"{label}: input_B '{input_b}' is not defined as a seed"
            )

        if input_a and input_b and input_a == input_b:
            errors.append(
                f"{label}: input_A and input_B are the same concept '{input_a}'. "
                f"A concept cannot interact with itself."
            )

        if "aliases" in i and not isinstance(i["aliases"], list):
            errors.append(f"{label}: 'aliases' must be an array")

    return errors


def main() -> None:
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <path-to-ontology.json>", file=sys.stderr)
        sys.exit(1)

    path = sys.argv[1]
    errors = validate(path)

    if errors:
        print(f"ontology validation FAILED: {path}")
        for e in errors:
            print(f"  ERROR: {e}")
        sys.exit(1)

    o = json.load(open(path))
    seed_count        = len(o.get("seeds", []))
    interaction_count = len(o.get("interactions", []))
    print(
        f"ontology.json valid: {seed_count} seeds, "
        f"{interaction_count} interactions — {path}"
    )


if __name__ == "__main__":
    main()
