package main

import (
	"fmt"
	"io/ioutil"
	"bufio"
	//"strings"
	"os"
	"path"
	"syscall"
	"math/big"
	//"math"
	//"crypto/aes"
	//"crypto/cipher"
	//"encoding/binary"
	//"encoding/hex"
	cryptor "crypto/rand"
	//mathr "math/rand"
	"time"
)

var flag string

var banner = `
                                             ϕϕϕϕ
                                             ϕϕϕϕ
                                            ϕϕϕϕ
                                            ϕϕϕϕ
                                           ϕϕϕϕϕ
                                     ϕϕϕϕϕϕϕϕϕϕϕϕϕϕϕϕ
                                  ϕϕϕϕϕϕϕ ϕϕϕϕϕ  ϕϕϕϕϕϕ
                                ϕϕϕϕϕ     ϕϕϕϕ     ϕϕϕϕϕ
                               ϕϕϕϕϕ      ϕϕϕϕ      ϕϕϕϕϕ
                              ϕϕϕϕϕ      ϕϕϕϕϕ      ϕϕϕϕϕ
                             ϕϕϕϕϕ       ϕϕϕϕ       ϕϕϕϕϕ
                             ϕϕϕϕ       ϕϕϕϕϕ      ϕϕϕϕϕ
                             ϕϕϕϕ       ϕϕϕϕ       ϕϕϕϕ
                             ϕϕϕϕ      ϕϕϕϕϕ     ϕϕϕϕϕ
                             ϕϕϕϕϕ     ϕϕϕϕ     ϕϕϕϕϕ
                              ϕϕϕϕϕϕ  ϕϕϕϕϕ  ϕϕϕϕϕϕ
                                ϕϕϕϕϕϕϕϕϕϕϕϕϕϕϕϕ
                                      ϕϕϕϕ
                                     ϕϕϕϕϕ
                                     ϕϕϕϕ
                                    ϕϕϕϕ
                                    ϕϕϕϕ   Corps

 ██╗███╗   ██╗███████╗██╗██╗  ████████╗██████╗  █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
 ██║████╗  ██║██╔════╝██║██║  ╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
 ██║██╔██╗ ██║█████╗  ██║██║     ██║   ██████╔╝███████║   ██║   ██║██║   ██║██╔██╗ ██║
 ██║██║╚██╗██║██╔══╝  ██║██║     ██║   ██╔══██╗██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
 ██║██║ ╚████║██║     ██║███████╗██║   ██║  ██║██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
 ╚═╝╚═╝  ╚═══╝╚═╝     ╚═╝╚══════╝╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
`


func main() {
	startup()

	fbuf, err := ioutil.ReadFile("./flag.txt");

	if err != nil {
		fmt.Fprintf(os.Stderr, "Unable to read flag: %s\n", err.Error())
		os.Exit(1)
	}
	flag = string(fbuf)

	fmt.Fprint(os.Stdout, `
Welcome Agent 65537!

Your deep cover has come through at last! You've been appointed to ϕ-Corps.
You've been given the designation Agent 65537 by the Corps.

The ϕ-Corps issues cryptographic keys to all their agents. This is our
chance to intercept messages for Agent 7.

As you know, after the great prime war of 25519 the Corps determined
that prime generation was too great of a risk for their agents. As
such, the ϕ-Corps now uses a shared modulus for all their agents with
a prime factorization known only to a select few at the Corps. The
Corps generates a random encrypting exponent and uses their knowledge
of the prime factorization of the shared modulus to derrive a
decrypting exponent. They issue such a pair of exponents to each of
their agents.

We've arranged for the Corps to issue you a keypair. We've intercepted
the public key for Agent 7 and you should be able to get their public key.
We need you to decrypt the message using your new appointment.
`)

	fmt.Fprintf(os.Stdout, "\n\nRetrieving key")

	p, err := cryptor.Prime(cryptor.Reader, 1024)

	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: unable to generate prime p\n")
		os.Exit(1)
	}

	fmt.Fprintf(os.Stdout, ".")

	q, err := cryptor.Prime(cryptor.Reader, 1024)


	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: unable to generate prime q\n")
		os.Exit(1)
	}

	fmt.Fprintf(os.Stdout, ".")

	n := new(big.Int)
	n.Mul(p, q)

	negone := big.NewInt(-1)
	pm1 := new(big.Int)
	qm1 := new(big.Int)
	pm1.Add(p, negone)
	qm1.Add(q, negone)

	etot := new(big.Int)
	etot.Mul(pm1, qm1)

	fmt.Fprintf(os.Stdout, ".")



	e, err := cryptor.Prime(cryptor.Reader, 512)
	d := new(big.Int)
	fmt.Fprintf(os.Stdout, ".")
	d.ModInverse(e, etot)
	fmt.Fprintf(os.Stdout, ".")

	if d == nil {
		fmt.Fprintf(os.Stderr, "Error: unable to generate d!\n")
		os.Exit(0)
	}


	e7, err := cryptor.Prime(cryptor.Reader, 512)
	d7 := new(big.Int)
	fmt.Fprintf(os.Stdout, ".")
	d7.ModInverse(e7, etot)
	fmt.Fprintf(os.Stdout, ".")

	if d7 == nil {
		fmt.Fprintf(os.Stderr, "Error: unable to generate d7!\n")
		os.Exit(0)
	}

	p7text, err := cryptor.Prime(cryptor.Reader, 512)
	fmt.Fprintf(os.Stdout, ".")
	c7text := new(big.Int)
	c7text.Exp(p7text, e7, n)
	fmt.Fprintf(os.Stdout, ".")

	rb, err := cryptor.Int(cryptor.Reader, big.NewInt(3))
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: stupid dot display loop failed!\n")
		os.Exit(1)
	}

	r := rb.Uint64() + 2
	for i := uint64(0); i < r; i++ {
		time.Sleep(2 * time.Second)
		fmt.Fprintf(os.Stdout, ".")
	}

	fmt.Fprintf(os.Stdout, "\n\nAgent 65537, we have your key:\n")
	fmt.Fprintf(os.Stdout, "ϕ-Corps public modulus: %s\n", n.Text(10))
	fmt.Fprintf(os.Stdout, "Your public encryption exponent: %s\n", e.Text(10))
	fmt.Fprintf(os.Stdout, "Your private decryption exponent: %s\n", d.Text(10))

	fmt.Fprintf(os.Stdout, "\nAgent 7's public encryption exponent is %s\n", e7.Text(10))
	fmt.Fprintf(os.Stdout, "The message we intercepted to Agent 7 was %s\n", c7text.Text(10))

	input := bufio.NewScanner(os.Stdin);
	fmt.Fprint(os.Stdout, "\nWhat was the message sent to Agent 7? ")
	ok := input.Scan()
	if !ok {
		fmt.Fprintln(os.Stdout, "Error reading input!")
		os.Exit(1);
	}

	atext := new(big.Int)
	atext.SetString(input.Text(), 10)

	if atext == nil {
		fmt.Fprintln(os.Stdout, "Error parsing answer!")
		os.Exit(1);
	}

	if atext.Cmp(p7text) == 0 {
		fmt.Fprintf(os.Stdout, "%s\n", flag)
	} else {
		fmt.Fprintln(os.Stdout, "Not correct! Your contract will be terminated.\n")
	}
}



func startup() {
	fmt.Fprint(os.Stdout, banner)
	changeBinDir()
	limitTime(20)
}


// Change to working directory
func changeBinDir() {
	// read /proc/self/exe
	if dest, err := os.Readlink("/proc/self/exe"); err != nil {
		fmt.Fprintf(os.Stderr, "Error reading link: %s\n", err)
		return
	} else {
		dest = path.Dir(dest)
		if err := os.Chdir(dest); err != nil {
			fmt.Fprintf(os.Stderr, "Error changing directory: %s\n", err)
		}
	}
}


// Limit CPU time to certain number of seconds
func limitTime(secs int) {
	lims := &syscall.Rlimit{
		Cur: uint64(secs),
		Max: uint64(secs),
	}
	if err := syscall.Setrlimit(syscall.RLIMIT_CPU, lims); err != nil {
		if inner_err := syscall.Getrlimit(syscall.RLIMIT_CPU, lims); inner_err != nil {
			fmt.Fprintf(os.Stderr, "Error getting limits: %s\n", inner_err)
		} else {
			if lims.Cur > 0 {
				// A limit was set elsewhere, we'll live with it
				return
			}
		}
		fmt.Fprintf(os.Stderr, "Error setting limits: %s", err)
		os.Exit(-1)
	}
}
