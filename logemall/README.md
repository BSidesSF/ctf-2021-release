# Challenge

Portraits script:

```sh
for i in (seq 1 151); convert (curl -s https://pokeapi.co/api/v2/pokemon/$i | jq | grep 'front_default.*png' | head -n1 | grep -o 'http.*png') -trim +repage jpg:- | ~/tools/jp2a-1.0.9/src/jp2a --width=32 --color-depth=8 - | grep -v '^$' | tee $i.txt; end
```

This is a use-after-free, ultimately. It's designed to simulate a vuln in the
original Pokemon games, where the encounter table can be corrupted and read
data from the player name instead.

# Build / deploy
