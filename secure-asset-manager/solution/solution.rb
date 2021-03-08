require 'tempfile'
require 'open3'
require 'socket'

HOST = "secure-asset-manager-4235c8a4.challenges.bsidessf.net"
PORT = 6112
CONNECTION_TIMEOUT = 60

def dump_binary(binary, target)
  Open3.popen2("gdb -q #{ binary }") do |i, o, t|
    # Breakpoint @ main
    i.puts("break malloc")

    # Run the executable
    i.puts("run")

    # Remove the breakpoint - this is VERY important, the breakpoint will mess
    # up the memory dump!
    i.puts("delete")

    # Get the pid
    i.puts("print (int) getpid()")

    loop do
      out = o.gets().strip()
      if out =~ /\$1 = ([0-9]+)/
        mappings = File.read("/proc/#{ $1 }/maps").split(/\n/)

        mappings.each do |m|
          if m =~ /([0-9a-f]+)-([0-9a-f]+) (r-xp).*\/secure-asset-manager$/
            i.puts("dump memory #{ target } 0x#{ $1 } 0x#{ $2 }")
            i.puts("quit")

            return
          end
        end
      end
    end
  end
end

def get_real_solution(checker, challenge_file, memory_file)
  cmd = "#{ checker } #{ challenge_file } #{ memory_file }"
  o, s = Open3.capture2(cmd)

  if !s.success?
    raise StandardError.new("Failed to calculate the correct solution!")
  end

  return o.to_i(16)
end

def do_read(s)
  header = s.read(2)
  if !header || header.length() != 2
    raise StandardError.new("Connection ended")
  end

  length = header.unpack('v').pop
  return s.read(length)
end

def do_write(s, data)
  s.write([data.length(), data].pack('va*'))
end

if !ARGV || !ARGV[0] || !ARGV[1]
  puts("Usage: ruby solution.rb <real executable> <path-to-not-solution-binary>");
  puts
  puts("real executable = The challenge file the user is given ('secure-asset-manager') - probably ../distfiles/secure-asset-manager")
  puts("not-solution binary = The server component not-solution, compiled - probably ../challenge/server/not-solution")
  exit(1)
end

# Dump the binary
begin
  dumped_binary = Tempfile.new('sam-solution-dump')
  dumped_binary.close()
  dump_binary(ARGV[0], dumped_binary.path)

  # Connect
  s = TCPSocket.new(HOST, PORT)
  puts("Server version: #{ do_read(s) }")
  do_write(s, "Solution v1.3.37\0")

  # Get challenge
  challenge = do_read(s)
  challenge_file = Tempfile.new('sam-solution-challenge')
  challenge_file.write(challenge)
  challenge_file.close()
  response = get_real_solution(ARGV[1], challenge_file.path, dumped_binary.path)
  puts "Sending challenge response"
  do_write(s, [response].pack('V'))

  # Check result
  puts("Result: #{ do_read(s) }")

  do_write(s, "HELP\0")
  puts("Help: #{ do_read(s) }")

  do_write(s, "CHECK_UPDATES\0")
  puts("Check updates:: #{ do_read(s) }")

  do_write(s, "I_AM solutiony.mc.solutionface\0")
  puts("I_AM: #{ do_read(s) }")

  do_write(s, "FLAG\0")
  puts("Flag: #{ do_read(s) }")

  do_write(s, "EASTER_EGG")
  egg = do_read(s)
  File.write("/tmp/easter-egg.jpg", egg)
  puts("Easter egg written to /tmp/easter-egg.jpg")
ensure
  if dumped_binary
    dumped_binary.close()
    dumped_binary.unlink()
  end

  if challenge_file
    challenge_file.close()
    challenge_file.unlink()
  end
end
