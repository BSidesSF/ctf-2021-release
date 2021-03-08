#pip install mersenne-twister-predictor
from mt19937predictor import MT19937Predictor
import fileinput
import re

i = fileinput.input()

def first_name(name):
    return FIRST_NAMES.index(name) if name in FIRST_NAMES else -1

def last_name(name):
    return LAST_NAMES.index(name) if name in LAST_NAMES else -1

def read_names(i):
    while True:
        line = next(i)
        if('-vs-' in line):
            break

    matches = re.search('([a-z]+)[ ]([a-z]*)[ ]+ -vs-  ([a-z]+) ([a-z]+)[^a-z]*$', line, re.IGNORECASE)

    a = first_name(matches.group(1))
    b = last_name(matches.group(2))
    c = first_name(matches.group(3))
    d = last_name(matches.group(4))

    a = (a | b << 16)
    b = (c | d << 16)

    return (a, b)

file = open('../challenge/first-names.txt', mode='r')
FIRST_NAMES = file.read().split("\n")
file.close()

file = open('../challenge/last-names.txt', mode='r')
LAST_NAMES = file.read().split("\n")
file.close()

file = open('../challenge/words.txt', mode='r')
WORDS = file.read().split("\n")
file.close()

print("Start a game with TEN (10) rounds, and paste the output (including player map-ups) here!")
matchups = []


# We're going to check 4 at a time
predictor = MT19937Predictor()
for _ in range(511):
    (a, b) = read_names(i)
    predictor.setrandbits(a, 32)
    predictor.setrandbits(b, 32)

# Make sure it's working
predicted = predictor.getrandbits(32)
first_predicted = FIRST_NAMES[predicted & 0x0000FFFF]
last_predicted = LAST_NAMES[predicted >> 16]
final_name_predicted = "%s %s" % (first_predicted, last_predicted)

(_, actual) = read_names(i)
first_actual = FIRST_NAMES[actual & 0x0000FFFF]
last_actual = LAST_NAMES[actual >> 16]
final_name_actual = "%s %s" % (first_actual, last_actual)

print("Validating...")
print(" -> Final name (predicted):", final_name_predicted)
print(" -> Final name (actual):   ", final_name_actual)
assert(final_name_predicted == final_name_actual)


print("Final names check out!")
print()
print("Here are the words, be sure to enter them without guesses (guesses consume random cycles):")

for i in range(10, 0, -1):
    word = predictor.getrandbits(32)
    print("Round %d: %s" % (10 - i + 1, WORDS[word & 0xFFFF]))

    # Waste RNG cycles
    for _ in range(1, (2**i) >> 1):
        predictor.getrandbits(64)

