#!/usr/bin/env python3
"""
Script to report line counts for all .txt filelists with optional filtering.
"""
import argparse
import sys
from pathlib import Path
from tabulate import tabulate


def get_filelist_info(directory):
    """
    Scan directory for .txt files and count their lines.
    
    Returns:
        List of tuples: (filename, line_count)
    """
    results = []
    txt_files = sorted(Path(directory).glob("*.txt"))
    
    for filepath in txt_files:
        try:
            with open(filepath, 'r') as f:
                line_count = sum(1 for _ in f)
            results.append((filepath.name, line_count))
        except Exception as e:
            print(f"Error reading {filepath.name}: {e}", file=sys.stderr)
    
    return results


def apply_filters(results, filters, anti_filters=None):
    """
    Filter results based on substring matching.
    
    Args:
        results: List of (filename, line_count) tuples
        filters: List of filter strings (include if any match)
        anti_filters: List of anti-filter strings (exclude if any match)
        
    Returns:
        Filtered list of (filename, line_count) tuples
    """
    filtered = results
    
    # Apply positive filters (include if any match)
    if filters:
        filtered = [item for item in filtered
                   if any(f in item[0] for f in filters)]
    
    # Apply negative filters (exclude if any match)
    if anti_filters:
        filtered = [item for item in filtered
                   if not any(f in item[0] for f in anti_filters)]
    
    return filtered


def main():
    parser = argparse.ArgumentParser(
        description="Report line counts for all .txt filelists with optional filtering"
    )
    parser.add_argument(
        "--filter",
        nargs="+",
        default=[],
        help="Only report files that contain any of these substrings"
    )
    parser.add_argument(
        "--anti-filter",
        nargs="+",
        default=[],
        help="Exclude files that contain any of these substrings"
    )
    parser.add_argument(
        "--max-lines",
        type=int,
        default=None,
        help="Only report files with fewer than this number of lines"
    )
    parser.add_argument(
        "--min-lines",
        type=int,
        default=None,
        help="Only report files with at least this number of lines"
    )
    
    args = parser.parse_args()
    
    # Get current directory of script
    script_dir = Path(__file__).parent
    
    # Get all filelist info
    results = get_filelist_info(script_dir)
    
    # Apply filters if specified
    results = apply_filters(results, args.filter, args.anti_filter)
    
    # Apply max-lines filter if specified
    if args.max_lines is not None:
        results = [item for item in results if item[1] < args.max_lines]

    # Apply min-lines filter if specified
    if args.min_lines is not None:
        results = [item for item in results if item[1] >= args.min_lines]
    
    if not results:
        print("No files found matching the criteria.")
        return
    
    # Sort by filename
    results.sort(key=lambda x: x[0])
    
    # Calculate totals
    total_lines = sum(count for _, count in results)
    
    # Prepare table data with totals row
    table_data = results + [("TOTAL", total_lines)]
    
    # Print table
    print(tabulate(table_data, headers=["Filename", "Lines"], tablefmt="grid", numalign="right"))
    
    print(f"\nTotal files: {len(results)}")
    print(f"Total lines: {total_lines}")


if __name__ == "__main__":
    main()
