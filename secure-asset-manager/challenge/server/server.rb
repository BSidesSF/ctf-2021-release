require 'open3'
require 'socket'
require 'timeout'

require 'singlogger'
L = SingLogger.instance()
SingLogger.set_level_from_string(level: "DEBUG")

require './assembly-generator.rb'

HOST = "0.0.0.0"
PORT = 6112
CONNECTION_TIMEOUT = 60
VERSION = "secure-asset-manager server v1.00"
FLAG = "CTF{this_is_kinda_like_the_starcraft_login}"

# Paths that can be customized with ENV (for easier dockering)
REAL_BINARY   = ENV['REAL_BINARY']   || "../client/secure-asset-manager"
DUMPED_BINARY = ENV['DUMPED_BINARY'] || "/tmp/secure-asset-manager-dump"
CHECKER       = ENV['CHECKER']       || "./not-solution"
EASTER_EGG    = ENV['EASTER_EGG']    || "./easter-egg.jpg"

def dump_binary(binary, target)
  L.info("Dumping memory from #{ binary } using gdb...")

  begin
    Timeout.timeout(3) do
      Open3.popen2("gdb -q #{ binary }") do |i, o, t|
        # Don't confirm things
        i.puts("set no-confirm")

        # Breakpoint @ malloc - we just need to stop anywhere
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
          puts(out)
          if out =~ /\$1 = ([0-9]+)/
            L.info("Found PID: #{ $1 }")
            L.info("Reading /proc/#{ $1 }/maps to find memory block")
            mappings = File.read("/proc/#{ $1 }/maps").split(/\n/)

            mappings.each do |m|
              if m =~ /([0-9a-f]+)-([0-9a-f]+) (r-xp).*\/secure-asset-manager$/
                L.debug("Found memory block: #{ m }")
                i.puts("dump memory #{ target } 0x#{ $1 } 0x#{ $2 }")
                i.puts("quit")

                loop do
                  out = o.gets()
                  if !out
                    break
                  end
                  puts(out.strip())
                end

                L.info("Memory from original binary dumped to #{ target }")
                return
              end
            end
          end
        end
      end
    end
  rescue Timeout::Error
    L.fatal("Something went wrong dumping the binary! Check the gdb output above")
    exit(1)
  end
end

def get_real_solution(checker, challenge_file, memory_file)
  cmd = "#{ checker } #{ challenge_file } #{ memory_file }"
  L.info("[THREAD #{ Thread.current.object_id }]: Executing solver: #{ cmd }")
  o, s = Open3.capture2(cmd)

  if !s.success?
    raise StandardError.new("Failed to calculate the correct solution!")
  end

  return o.to_i(16)
end

def do_read(s)
  L.debug("[THREAD #{ Thread.current.object_id }]: Reading 2-byte header")
  header = s.read(2)
  if !header || header.length() != 2
    raise StandardError.new("Connection ended")
  end

  length = header.unpack('v').pop
  L.debug("[THREAD #{ Thread.current.object_id }]: Reading #{ length } bytes")

  return s.read(length)
end

def do_write(s, data)
  L.debug("[THREAD #{ Thread.current.object_id }]: Sending #{ data.length } bytes")

  s.write([data.length(), data].pack('va*'))
end

def handle_legit_client(s)
  loop do
    data = do_read(s)

    if data.start_with?('CHECK_UPDATES')
      # No updates
      L.info("[THREAD #{ Thread.current.object_id }]: Client is checking for updates")
      do_write(s, "0 :: No updates available\0")
    elsif data.start_with?('I_AM')
      L.info("[THREAD #{ Thread.current.object_id }]: Client is checking in :: #{ data }")
      do_write(s, "OK :: Checkin successful! For more commands, send HELP\0");
    elsif data.start_with?('EASTER_EGG')
      L.info("[THREAD #{ Thread.current.object_id }]: Client wants the easter egg")
      do_write(s, File.binread('easter-egg.jpg'));
    elsif data.start_with?('FLAG')
      L.info("[THREAD #{ Thread.current.object_id }]: Client wants the flag")
      do_write(s, "OK :: #{ FLAG }\0");
    elsif data.start_with?('HELP')
      L.info("[THREAD #{ Thread.current.object_id }]: Client wants help")
      do_write(s, "OK :: CHECK_UPDATES :: I_AM <hostname> :: FLAG :: HELP :: EASTER_EGG\0");
    elsif data.start_with?('BYE')
      L.info("[THREAD #{ Thread.current.object_id }]: Client wants to exit")
      do_write(s, "OK :: BYE!\0");
      return
    else
      L.info("[THREAD #{ Thread.current.object_id }]: Bad command: #{ data }")
      do_write(s, "ERROR :: Unknown command\0");
    end
  end
end

def handle_client(s)
  begin
    L.debug("[THREAD #{ Thread.current.object_id }]: Sending version info")
    do_write(s, VERSION + "\0")

    L.debug("[THREAD #{ Thread.current.object_id }]: Waiting for their version info")
    version = do_read(s)

    L.info("[THREAD #{ Thread.current.object_id }]: Client version: #{ version }")

    L.debug("[THREAD #{ Thread.current.object_id }]: Generating client challenge")
    challenge_file = generate_challenge()
    challenge_data = File.binread(challenge_file.path)

    L.info("[THREAD #{ Thread.current.object_id }]: Sending #{ challenge_data.length }-byte challenge")
    do_write(s, challenge_data)

    L.info("[THREAD #{ Thread.current.object_id }]: Waiting for challenge response")
    challenge_response = do_read(s)

    if challenge_response.length != 4
      L.warn("[THREAD #{ Thread.current.object_id }]: Invalid challenge response length; expected 4 bytes, found #{ challenge_response.length } bytes!")
      do_write(s, "FAIL")
      return
    end

    # Unpack as little endian
    their_solution = challenge_response.unpack('V').pop

    # Get the correct value (we wait at late as possible to avoid wasting server time)
    correct_solution = get_real_solution(CHECKER, challenge_file.path, DUMPED_BINARY)
    #correct_solution = get_real_solution(CHECKER, "/home/ron/tmp/test", DUMPED_BINARY)

    L.info("[THREAD #{ Thread.current.object_id }]: Their checksum = 0x%08x, correct checksum = 0x%08x" % [their_solution, correct_solution])

    if their_solution != correct_solution
      do_write(s, 'FAIL')
      return
    end

    L.info("[THREAD #{ Thread.current.object_id }]: Client authorized!")
    do_write(s, 'PASS')
    handle_legit_client(s)
  ensure
    if challenge_file
      challenge_file.unlink()
    end
  end
end

# Dump the binary
dump_binary(REAL_BINARY, DUMPED_BINARY)

L.info("Starting TCP server on #{ HOST }:#{ PORT }")
s = TCPServer.new(HOST, PORT)
loop do
  client = s.accept()
  L.info("Received connection from #{ client.peeraddr[2] }:#{client.peeraddr[1]}")

  Thread::new() do |t|
    L.info("[THREAD #{ Thread.current.object_id }]: Spawned handler thread")
    begin
      Timeout.timeout(CONNECTION_TIMEOUT) do
        handle_client(client)
        L.info("[THREAD #{ Thread.current.object_id }]: Connection ended cleanly")
      end
    rescue Exception => e
      L.error("[THREAD #{ Thread.current.object_id }]: Something went wrong with the thread: #{ e }")
    ensure
      client.close()
    end
  end
end
