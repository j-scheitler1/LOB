#!/usr/bin/env sh
set -eu

bundle="order_book_replay_bundle.tar.gz"
files="
order_book_replay.html
order_book_replay.css
order_book_replay.js
order_book_data.js
"

missing=0
for file in $files; do
  if [ ! -f "$file" ]; then
    printf 'missing: %s\n' "$file" >&2
    missing=1
  fi
done

if [ "$missing" -ne 0 ]; then
  printf '\nRun `make visuals` first if order_book_data.js is missing.\n' >&2
  exit 1
fi

tar -czf "$bundle" $files

printf 'Created %s\n' "$bundle"
printf 'Copy it with: scp user@server:/path/to/LOB/%s .\n' "$bundle"
printf 'Then extract it with: tar -xzf %s\n' "$bundle"
printf 'Open order_book_replay.html in a browser.\n'
