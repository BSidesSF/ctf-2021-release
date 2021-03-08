package main

import (
	"bufio"
	"bytes"
	cryptor "crypto/rand"
	"encoding/hex"
	"fmt"
	"io/ioutil"
	"os"
	"path"
	"permhash296"
	"regexp"
	"strings"
	"syscall"
)

var (
	flag_easy, flag_med, flag_hard string
	permhash_code                  string
)

var re_hex_str = regexp.MustCompile(`^\s*"?(?:[0\\]x)?((?:[0-9a-fA-F]{2})*)"?\s*$`)

const challenge_count = 8

var challenge_status [challenge_count]bool

const prog_name = "PermCoin Hacktool"

const help_text = `
Commands:
    help                // Prints this help
    help permcoin       // Describe PermCoin and the attacks
    help permhash       // Describe permhash296
    help conventions    // Describe the conventions used by this attack tool
    help code           // Dump sample permhash296 code

    hash "<hex string>" // Compute the permhash296 of the provided hex-encoded string

    status              // Print the attack challenge status
    challenge <n>       // Complete the nth challenge

    attack starter      // Run the starter attack against PermCoin
    attack partial      // Run the partial attack against PermCoin
    attack full         // Run the full attack against PermCoin

    exit                // Exit the attack tool
    ^d                  // Same as exit but 100% more Unix-points
`

const help_permcoin_text = `
PermCoin is the ultimate pinnacle in cryptocurrencies today! It's
visionary authors dared to dream of something greater than has ever
come before; and then they went beyond even that dream. Built on the
belief that trinary is good but binary is better, PermCoin manages to
achieve what others thought was impossible.

At the heart of PermCoin is an all-new hash algorithm: permhash296.
With 296 bits of raw power it's more than 4.9 times better than the
Nintendo 64! permhash296 manages to pack an industry-first 296 bits of
security into only 296 bits of space.

PermCoin is the inevitable convergence of the co-evolution of the
blockchain revolution and hash algorithms.
`

const help_permhash_text = `
permhash296 is a revolution in the evolution of ever-evolving
revolutionary technologies!

permhash296 thinks big. Really BIG. It's like 256 but 40 more BIG.

The design of permhash296 does away with all preconceived conventions
and throws those preconceptions out the window! Its revolutionary
design makes it impervious to all previous attacks.

At the heart of permhash296's genius are a few simple key observations:

* Rubik's Cubes are awesome
* Hypercubes are awesome
* The 6-cube has 64 hyper-vertices of awesome in hyperspace
* If somehow a hash could leverage this, the hash would be awesome

With these key observations the permhash296 designers set out to pack
as much awesome as can fit in 296 bits.

Per the name, permhash296 is built on permutations of its 64-element
state-space. Each permutation was artisanally hand-crafted by the
authors to pack the maximum amount of permuting possible into a
permutation. As input is consumed this state space gets impossibly
scrambled. Then, when the input is finished, the state space gets
scrambled EVEN MORE. It's like you scrambled a Rubik's Cube and then
scrambled it even more. The security level is RIDICULOUS.

But don't take our word for it, we have three flags riding on these
claims!
`

const help_conventions_text = `
This tool uses hexadecimal encoding for all input and output.  If you
want to hash the 4-byte string "test" you must encode it in hex as
74657374. permhash296 operates on bytes so all hex-encoded strings
must be an even number of digits.

You can enter the empty-string as "".

The || operator is concatenation.  41||6243 is 416243 which represents the
3-byte text string "AbC".
`

const challenge_1_text = `
If we're going to have any chance of attacking the PermCoin blockchain
we'll need to be able to compute permhash296 on arbitrary inputs! To
make sure we have the permhash296 computations working, you will be
asked to compute the permhash296 of a random hex-encoded string.

== Technical Requirements ==
permhash296(<provided plaintext>) = <your answer>
`
const challenge_2_text = `
PermCoin was built with the assumption that permhash296 is
collision-resistant.  If we can find collisions in permhash296 we can
'fork' the blockchain by sending some nodes in the network one copy
and other nodes in the network the other copy without worrying about
hash inconsistencies. Later when the nodes try to reconcile, the chain
will validate cryptographically but their history will disagree. We've
found a way to crash the PermCoin client during reconciliation with
collisions. Using this we can temporarily DoS the network and perform
arbitrage on the price differential when the network can't settle!

== Technical Requirements ==
permhash296(<your message 1>) = permhash296(<your message 2>)
`
const challenge_3_text = `
There are a number of situations where having an input that hashes to
the same value as the empty-string/empty-input is useful. Trivial
collisions are possible with such a value! We will need an input that
collides with the empty-string in order to simplify our attacks.

== Technical Requirements ==
permhash296("") = permhash296(<your message>)
`

const challenge_4_text = `
As part of a new PermCoin transparency initiative, central nodes
broadcast a 'running hash' of transactions without broadcasting the
transactions themselves.  Later when the block is 'sealed' with a
final hash the transactions are revealed and nodes can verify the
previously-broadcast running hashes.  To append rogue transactions
into the ledger we need to be able to compute permhash296 values from
these intermediate hashes without having the intermediate
plaintext. Our best bet is a length-extension attack.

== Technical Requirements ==
permhash296(<plaintext not provided>) = <given hash>
permhash296(<plaintext not provided>||<given message>) = <your answer>
`

const challenge_5_text = `
For permhash296 collisions to be maximally useful, we need you to be
able to create very short ones of equal length. Furthermore, some byte
comparison code in the PermCoin client will erroneously handle our
collisions if they have even a single byte in common at the same
place.

For example \x0011 compared to \x1122 is fine but \x0011 compared to
\x0022 is not because they both start with \x00 which is the same byte
in the same offset.

We will need you to find a pair of equal-length short messages that
don't share any common bytes in the same place that have the same
permhash296 value. This will allow us to send PermCoins to specific
nodes in the network and then later we can swap-out the node ID in the
ledger with a colliding node ID we control. With this, we can spend
money and then later re-write history to pay ourselves!

== Technical Requirements ==
permhash296(<your message 1>) = permhash296(<your message 2>)
`

const challenge_6_text = `
We think we've found a way to spend or transfer PermCoins that we no
longer own!  Up until now the ledger history stored in the PermCoin
blockchain has prevented any significant attacks. If you can generate
an entirely fake history that matches the permhash296 hash up to some
point we can swap out the start of the real ledger for our (possibly
corrupt) false initial history.  This attack will be detected
eventually but if we convert our stollen PermCoins to fiat before it's
detected, nothing can be done.

To pull this attack off, we'll find a permhash296 value in the
blockchain history. Your job will be to find some input (a pre-image)
that hashes to that same hash.

== Technical Requirements ==
permhash296(<your answer plaintext>) = <given hash>
`
const challenge_7_text = `
To maintain multiple fake ledger histories without mounting an
expensive 51% attack we need to be able to converge our alternate
histories into the same hash value so that as the network can keeps
appending to the public blockchain, that work continues to apply to our
fake histories.

In order for this to work, we need you to find chosen-prefix
collisions. Also, these two messages, including the prefix, must be
8192 bytes or less.

== Technical Requirements ==
permhash296(<given prefix 1>||<your message 1>) = permhash296(<given prefix 2>||<your message 2>)
`

const challenge_8_text = `
This is it. The final attack. If you can pull this off we can
manipulate the PermCoin blockchain in any way we want and we'll never
be detected. All of our previous history re-writing attacks have been
rather heavy-handed and replaced the entire ledger history before some
point.  But if we could just re-write a middle section of the ledger
we'd have a precision money^H^H^H^H^Hflag making tool.

Here is what we need: for a given permhash296 we need you to
length-extend the plaintext to produce a new hash of our choice.
Here's the catch: your appended message must be exactly 1024 bytes or
it won't fit where we need it to in the transaction list.

With this ability we'll be able to precisely alter/overwrite/insert
any transaction we want anywhere into the PermCoin history.

== Technical Requirements ==
permhash296(<plaintext not provided>) = <given starting hash>
permhash296(<plaintext not provided>||<your answer plaintext>) = <given goal hash>
`

func main() {
	startup()

	input := bufio.NewScanner(os.Stdin)

	fmt.Fprint(os.Stdout, "\nWelcome Crytographer!\nTogether we're going to steal lots of money^H^H^H^H^Hflags...\n")
	fmt.Fprint(os.Stdout, "\nTry \"help\" for a list of commands\n")

	exit := false

	for !exit {
		fmt.Fprintf(os.Stdout, "\n%s> ", prog_name)
		ok := input.Scan()
		if !ok {
			fmt.Fprintln(os.Stdout, "")
			break
		}

		text := input.Text()

		if len(text) == 0 {
			continue
		}
		//fmt.Fprintf(os.Stdout, "Got command: %s\n", text)

		tokens := strings.Split(text, " ")

		switch tokens[0] {

		case "help":
			if len(tokens) > 1 {
				switch tokens[1] {

				case "permcoin":
					print_help_permcoin()

				case "permhash":
					print_help_permhash()

				case "permhash296":
					print_help_permhash()

				case "conventions":
					print_help_conventions()

				case "code":
					print_permhash_code()

				default:
					print_help()
				}

			} else {
				print_help()
			}

		case "hash":
			if len(tokens) == 2 {
				var phash permhash296.Permhash296
				fmt.Fprintf(os.Stdout, "%s\n", phash.Hash_hex(hex_to_bytes(tokens[1])))

			} else {
				fmt.Fprintf(os.Stdout, "hash command requires one hex-encoded argument. Try \"help\" for a list of commands.")
			}

		case "challenge":
			if len(tokens) == 2 {
				switch tokens[1] {

				case "1": // =========================== 1 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 1:\n%s\n", challenge_1_text)

					c1 := rand_bytes(8)
					phash.Hash(c1)
					a1 := phash.Tobinary()
					fmt.Fprintf(os.Stdout, "Please compute the hash of %s", hex.EncodeToString(c1))
					fmt.Fprintf(os.Stdout, "\npermhash296? ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					h1 := hex_to_bytes(input.Text())

					if bytes.Equal(h1, a1) {
						challenge_status[0] = true
						fmt.Fprintf(os.Stdout, "\nChallenge 1 complete!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nIncorrect! Challenge 1 failed.\n")
					}

				case "2": // =========================== 2 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 2:\n%s\n", challenge_2_text)

					fmt.Fprintf(os.Stdout, "Please enter message 1: ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					i1 := input.Text()
					b1 := hex_to_bytes(i1)
					phash.Hash(b1)
					h1 := phash.Tobinary()
					fmt.Fprintf(os.Stdout, "Message 1's permhash296 is %s\n", hex.EncodeToString(h1))

					fmt.Fprintf(os.Stdout, "\nPlease enter message 2 (this must hash to the same thing as message 1): ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					i2 := input.Text()
					b2 := hex_to_bytes(i2)
					phash.Hash(b2)
					h2 := phash.Tobinary()

					fmt.Fprintf(os.Stdout, "Message 2's permhash296 is %s\n", hex.EncodeToString(h2))

					if bytes.Equal(h1, h2) {
						if bytes.Equal(b1, b2) {
							fmt.Fprintf(os.Stdout, "That's cute... Your two inputs hash to the same value because your inputs were the same value!\n")
							fmt.Fprintf(os.Stdout, "Try again...\n")
						} else {
							challenge_status[1] = true
							fmt.Fprintf(os.Stdout, "\nChallenge 2 complete!\n")
						}
					} else {
						fmt.Fprintf(os.Stdout, "\nThe hashes of your inputs don't match! Challenge 2 failed.\n")
					}

				case "3": // =========================== 3 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 3:\n%s\n", challenge_3_text)

					phash.Hash([]byte(""))
					e1 := phash.Tobinary()
					fmt.Fprintf(os.Stdout, "The empty-string hash: %s\n", hex.EncodeToString(e1))

					fmt.Fprintf(os.Stdout, "\nPlease another message that collides with the empty-string: ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					b1 := hex_to_bytes(input.Text())
					phash.Hash(b1)
					h1 := phash.Tobinary()
					fmt.Fprintf(os.Stdout, "Your input's permhash296 is %s\n", hex.EncodeToString(h1))

					if bytes.Equal(e1, h1) {
						if bytes.Equal([]byte(""), b1) {
							fmt.Fprintf(os.Stdout, "That's cute... You provided the emtpy string!\n")
							fmt.Fprintf(os.Stdout, "Try again...\n")
						} else {
							challenge_status[2] = true
							fmt.Fprintf(os.Stdout, "\nChallenge 3 complete!\n")
						}
					} else {
						fmt.Fprintf(os.Stdout, "\nThe hash of your input doesn't match the empty-string hash! Challenge 3 failed.\n")
					}

				case "4": // =========================== 4 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 4:\n%s\n", challenge_4_text)

					p1 := rand_bytes(128)
					phash.Hash(p1)
					ph1 := phash.Tobinary()

					ap1 := rand_bytes(8)

					phash.Hash(append(p1, ap1...))
					a1 := phash.Tobinary()

					fmt.Fprintf(os.Stdout, "The intermediate hash broadcast through the network was %s\n", hex.EncodeToString(ph1))
					fmt.Fprintf(os.Stdout, "We need you to compute a new intemediate hash after %s has been appended to the hidden plaintext.\n", hex.EncodeToString(ap1))

					fmt.Fprintf(os.Stdout, "\nWhat is the new length-extended hash? ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					b1 := hex_to_bytes(input.Text())

					if bytes.Equal(b1, a1) {
						challenge_status[3] = true
						fmt.Fprintf(os.Stdout, "\nChallenge 4 complete!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nThe hash you provided doesn't match the correct hash of the appended string: %s\n", hex.EncodeToString(a1))
						fmt.Fprintf(os.Stdout, "\nChallenge 4 failed!\n")
					}

				case "5": // =========================== 5 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 5:\n%s\n", challenge_5_text)

					fmt.Fprintf(os.Stdout, "Please enter message 1 (max 8 bytes): ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					i1 := input.Text()
					b1 := hex_to_bytes(i1)

					if len(b1) > 8 {
						fmt.Fprintf(os.Stdout, "Your message is too long to be useful!\n")
						break
					}

					phash.Hash(b1)
					h1 := phash.Tobinary()
					fmt.Fprintf(os.Stdout, "Message 1's permhash296 is %s\n", hex.EncodeToString(h1))

					fmt.Fprintf(os.Stdout, "\nPlease enter message 2 (length %d bytes): ", len(b1))

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					i2 := input.Text()
					b2 := hex_to_bytes(i2)

					if len(b1) != len(b2) {
						fmt.Fprintf(os.Stdout, "Message 2 must be the same length as message 1!\n")
						break
					}

					if len(b1) == 0 {
						fmt.Fprintf(os.Stdout, "The empty string input is useless for collisions!\n")
						break
					}

					bmatch := false
					for i := 0; i < len(b1); i++ {
						if b1[i] == b2[i] {
							bmatch = true
							break
						}
					}

					if bmatch {
						fmt.Fprintf(os.Stdout, "Messages 1 and 2 can't have any bytes in common in the same position!\n")
						break
					}

					phash.Hash(b2)
					h2 := phash.Tobinary()

					fmt.Fprintf(os.Stdout, "Message 2's permhash296 is %s\n", hex.EncodeToString(h2))

					if bytes.Equal(h1, h2) {
						challenge_status[4] = true
						fmt.Fprintf(os.Stdout, "\nChallenge 5 complete!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nThe hashes of your inputs don't match! Challenge 5 failed.\n")
					}

				case "6": // =========================== 6 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 6:\n%s\n", challenge_6_text)

					r1 := rand_bytes(1024)
					phash.Hash(r1)
					a1 := phash.Tobinary()
					fmt.Fprintf(os.Stdout, "Please compute a pre-image for the hash %s", hex.EncodeToString(a1))
					fmt.Fprintf(os.Stdout, "\npre-image? ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					b1 := hex_to_bytes(input.Text())
					phash.Hash(b1)
					h1 := phash.Tobinary()

					if bytes.Equal(h1, a1) {
						challenge_status[5] = true
						fmt.Fprintf(os.Stdout, "\nChallenge 6 complete!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nYour pre-image hashes to the wrong value! Challenge 6 failed.\n")
					}

				case "7": // =========================== 7 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 7:\n%s\n", challenge_7_text)

					p1 := rand_bytes(20)
					p2 := rand_bytes(20)

					fmt.Fprintf(os.Stdout, "We've found two 20-byte prefixes.\nPrefix 1: %s\nPrefix 2: %s\n\n", hex.EncodeToString(p1), hex.EncodeToString(p2))
					fmt.Fprintf(os.Stdout, "Please find two messages to append, one to prefix 1,\nand the other to prefix 2, such that the permhhash296 is the same for both.\n")

					fmt.Fprintf(os.Stdout, "\nAppend message 1 (limit 8172 bytes)? ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					ap1 := hex_to_bytes(input.Text())

					if len(ap1) > 8172 {
						fmt.Fprintf(os.Stdout, "Message 1 must be less than 8172 bytes!\n")
						break
					}

					phash.Hash(append(p1, ap1...))
					h1 := phash.Tobinary()

					fmt.Fprintf(os.Stdout, "Chosen prefix 1 + message 1 hash: %s\n", hex.EncodeToString(h1))

					fmt.Fprintf(os.Stdout, "\nAppend message 2 (limit 8172 bytes)? ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					ap2 := hex_to_bytes(input.Text())

					if len(ap2) > 8172 {
						fmt.Fprintf(os.Stdout, "Message 2 must be less than 8172 bytes!\n")
						break
					}

					phash.Hash(append(p2, ap2...))
					h2 := phash.Tobinary()

					fmt.Fprintf(os.Stdout, "Chosen prefix 2 + message 2 hash: %s\n", hex.EncodeToString(h2))

					if bytes.Equal(h1, h2) {
						challenge_status[6] = true
						fmt.Fprintf(os.Stdout, "\nChallenge 7 complete!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nYour messages don't collide when appended to their respective prefixes!\nChallenge 7 failed!\n")
					}

				case "8": // =========================== 8 ============================
					var phash permhash296.Permhash296
					fmt.Fprintf(os.Stdout, "Challenge 8:\n%s\n", challenge_8_text)

					p1 := rand_bytes(128)
					phash.Hash(p1)
					ph1 := phash.Tobinary()

					g1 := rand_bytes(128)
					phash.Hash(g1)
					gh1 := phash.Tobinary()

					fmt.Fprintf(os.Stdout, "The intermediate hash so far is %s\n", hex.EncodeToString(ph1))
					fmt.Fprintf(os.Stdout, "We need you to length-extend the plaintext for this hash with 1024 more bytes so that the new hash is %s\n", hex.EncodeToString(gh1))

					fmt.Fprintf(os.Stdout, "\nYour found 1024-byte transaction? ")

					ok = input.Scan()
					if !ok {
						fmt.Fprintln(os.Stdout, "")
						exit = true
						break
					}

					i1 := hex_to_bytes(input.Text())

					if len(i1) != 1024 {
						fmt.Fprintf(os.Stdout, "Your appended transaction must be exactly 1024 bytes!\n")
						break
					}

					phash.Hash(append(p1, i1...))
					gh2 := phash.Tobinary()

					if bytes.Equal(gh1, gh2) {
						challenge_status[7] = true
						fmt.Fprintf(os.Stdout, "\nChallenge 8 complete!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nThe appended 1024-byte transaction provided doesn't match the correct goal hash. Yours hashed to: %s\n", hex.EncodeToString(gh2))
						fmt.Fprintf(os.Stdout, "\nChallenge 8 failed!\n")
					}

					// =========================== END ============================
				default:
					fmt.Fprintf(os.Stdout, "\"%s\" argument not understood. challenge command requires one numeric argument. Try \"help\" for a list of commands.", tokens[1])
				}

			} else {
				fmt.Fprintf(os.Stdout, "challenge command requires one numeric argument. Try \"help\" for a list of commands.")
			}

		case "attack":
			if len(tokens) == 2 {
				switch tokens[1] {

				case "starter":
					if challenge_status[0] {
						fmt.Fprintf(os.Stdout, "\nChallenge 1 complete, hash computations computing!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 1 incomplete. Starter attack not possible!\n")
						break
					}

					if challenge_status[1] {
						fmt.Fprintf(os.Stdout, "\nChallenge 2 complete, collisions colliding!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 2 incomplete. Starter attack not possible!\n")
						break
					}

					if challenge_status[2] {
						fmt.Fprintf(os.Stdout, "\nChallenge 3 complete, empty strings emptying!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 3 incomplete. Starter attack not possible!\n")
						break
					}

					fmt.Fprintf(os.Stdout, "\nStarter attack complete!\n%s\n", flag_easy)

				case "partial":
					if challenge_status[3] {
						fmt.Fprintf(os.Stdout, "\nChallenge 4 complete, lengths extending!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 4 incomplete. Partial attack not possible!\n")
						break
					}

					if challenge_status[4] {
						fmt.Fprintf(os.Stdout, "\nChallenge 5 complete, collisions restricted!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 5 incomplete. Partial attack not possible!\n")
						break
					}

					if challenge_status[5] {
						fmt.Fprintf(os.Stdout, "\nChallenge 6 complete, images pre-imaging!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 6 incomplete. Partial attack not possible!\n")
						break
					}

					fmt.Fprintf(os.Stdout, "\nPartial attack complete!\n%s\n", flag_med)

				case "full":
					if challenge_status[6] {
						fmt.Fprintf(os.Stdout, "\nChallenge 7 complete, chosen collisions prefixed!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 7 incomplete. Full attack not possible!\n")
						break
					}

					if challenge_status[7] {
						fmt.Fprintf(os.Stdout, "\nChallenge 8 complete, state optimally optimized!\n")
					} else {
						fmt.Fprintf(os.Stdout, "\nChallenge 8 incomplete. Full attack not possible!\n")
						break
					}

					fmt.Fprintf(os.Stdout, "\nFull attack complete!\n%s\n", flag_hard)

				default:
					fmt.Fprintf(os.Stdout, "\"%s\" argument not understood. attack command requires one of {starter, partial, full}. Try \"help\" for a list of commands.", tokens[1])

				}

			} else {
				fmt.Fprintf(os.Stdout, "challenge command requires one numeric argument. Try \"help\" for a list of commands.")
			}

		case "status":
			print_status()

		case "h":
			print_help()

		case "?":
			print_help()

		case "exit":
			exit = true

		case "quit":
			exit = true

		case "flag":
			fmt.Fprintf(os.Stdout, "lolz you typed 'flag' but that isn't a command. You didn't really think that was going to work, did you?\n")
			exit = true

		case "^d":
			fmt.Fprintf(os.Stdout, "Uhmmm... You do realize that the '^' in '^d' isn't a literal '^' right??")

		default:
			fmt.Fprintf(os.Stdout, "%s: `%s` command not found. Try \"help\" for a list of commands.", prog_name, tokens[0])

		}
	}

}

func print_status() {
	fmt.Fprintf(os.Stdout, "\nChallenge Statuses\n")
	fmt.Fprintf(os.Stdout, "==================\n\n")

	fmt.Fprintf(os.Stdout, "Starter:\n")

	fmt.Fprintf(os.Stdout, " 1 - ")
	if challenge_status[0] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 arbitrary-input computation\n")

	fmt.Fprintf(os.Stdout, " 2 - ")
	if challenge_status[1] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 collision\n")

	fmt.Fprintf(os.Stdout, " 3 - ")
	if challenge_status[2] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 empty-string collision\n")

	fmt.Fprintf(os.Stdout, "\nPartial:\n")

	fmt.Fprintf(os.Stdout, " 4 - ")
	if challenge_status[3] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 length extension\n")

	fmt.Fprintf(os.Stdout, " 5 - ")
	if challenge_status[4] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 restricted collision\n")

	fmt.Fprintf(os.Stdout, " 6 - ")
	if challenge_status[5] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 unrestricted pre-image\n")

	fmt.Fprintf(os.Stdout, "\nFull:\n")

	fmt.Fprintf(os.Stdout, " 7 - ")
	if challenge_status[6] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 chosen-prefix collision\n")

	fmt.Fprintf(os.Stdout, " 8 - ")
	if challenge_status[7] {
		fmt.Fprintf(os.Stdout, "<complete> ")
	}
	fmt.Fprintf(os.Stdout, "permhash296 optimized state control\n")

}

func print_help() {
	fmt.Fprintf(os.Stdout, "\n%s help:\n%s", prog_name, help_text)
}

func print_help_permcoin() {
	fmt.Fprintf(os.Stdout, "%s", help_permcoin_text)
}

func print_help_permhash() {
	fmt.Fprintf(os.Stdout, "%s", help_permhash_text)
}

func print_help_conventions() {
	fmt.Fprintf(os.Stdout, "%s", help_conventions_text)
}

func print_permhash_code() {
	fmt.Fprint(os.Stdout, "\n// === START SAMPLE PERMHASH296 GO CODE ===\n")
	fmt.Fprintf(os.Stdout, "%s", permhash_code)
	fmt.Fprint(os.Stdout, "\n// === END SAMPLE PERMHASH296 GO CODE ===\n")
}

func hex_to_bytes(s string) []byte {

	var hex_str string
	matches := re_hex_str.FindStringSubmatch(s)

	if matches != nil {
		hex_str = matches[1]
	} else {
		os.Exit(-1)
	}

	b, err := hex.DecodeString(hex_str)

	if err != nil {
		os.Exit(-1)
	}

	return b
}

func rand_bytes(n int) []byte {

	b := make([]byte, n)

	_, err := cryptor.Read(b)

	if err != nil {
		os.Exit(-1)
	}

	return b
}

func startup() {

	changeBinDir()
	limitTime(5)

	for i := 0; i < challenge_count; i++ {
		challenge_status[i] = false
	}

	bannerbuf, err := ioutil.ReadFile("./banner.txt")

	if err != nil {
		fmt.Fprintf(os.Stderr, "Unable to read banner: %s\n", err.Error())
		os.Exit(1)
	}
	fmt.Fprint(os.Stdout, string(bannerbuf))

	fbuf, err := ioutil.ReadFile("./flag_easy.txt")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Unable to read flag_easy: %s\n", err.Error())
		os.Exit(1)
	}
	flag_easy = string(fbuf)

	fbuf, err = ioutil.ReadFile("./flag_med.txt")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Unable to read flag_med: %s\n", err.Error())
		os.Exit(1)
	}
	flag_med = string(fbuf)

	fbuf, err = ioutil.ReadFile("./flag_hard.txt")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Unable to read flag_hard: %s\n", err.Error())
		os.Exit(1)
	}
	flag_hard = string(fbuf)

	fbuf, err = ioutil.ReadFile("./src/permhash296/permhash296.go")
	if err != nil {
		fmt.Fprintf(os.Stderr, "Unable to read permhash296.go: %s\n", err.Error())
		os.Exit(1)
	}
	permhash_code = string(fbuf)
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
