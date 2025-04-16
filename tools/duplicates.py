import sys
import pandas as pd
from difflib import SequenceMatcher
from pathlib import Path


def similar(a: str, b: str, threshold: float = 0.8) -> bool:
    """
    Return True if the similarity ratio between strings a and b is at least threshold.
    """
    return SequenceMatcher(None, a, b).ratio() >= threshold


def main():
    csv_path = Path.cwd() / sys.argv[-1]
    if csv_path.suffix != '.csv' or not csv_path.exists():
        print('usage python duplicates.py <csv>')
        sys.exit()

    df = pd.read_csv(csv_path)
    names = df['name'].astype(str).tolist()

    # Similarity threshold
    threshold = 0.9
    duplicates = []
    n = len(names)
    # Compare each pair of names
    for i in range(n):
        for j in range(i + 1, n):
            score = SequenceMatcher(None, names[i], names[j]).ratio()
            if score >= threshold:
                # CSV line numbers: header is line 1, so data rows start at line 2
                line_i = i + 2
                line_j = j + 2
                duplicates.append((names[i], line_i, names[j], line_j, score))

    # Report results
    if duplicates:
        print(f"Similar or duplicated names found (threshold = {threshold:.2f}):")
        for name1, li, name2, lj, score in duplicates:
            print(f"  Line {li} {name1!r} <> Line {lj} {name2!r}  (similarity = {score:.2f})")
    else:
        print("No similar duplicate names found.")


if __name__ == '__main__':
    main()
