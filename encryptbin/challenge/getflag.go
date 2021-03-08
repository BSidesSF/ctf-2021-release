package main

import (
	"fmt"
	"io"
	"os"
)

var flag_paths = []string{
	"./flag.txt",
	"/home/flag/flag.txt",
}

func main() {
	for _, p := range flag_paths {
		if fp, err := os.Open(p); err == nil {
			io.Copy(os.Stdout, fp)
			return
		}
	}
	fmt.Println("Unable to find flag file!")
}
