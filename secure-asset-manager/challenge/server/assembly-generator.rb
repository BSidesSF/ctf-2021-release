require 'tempfile'

NOPS = [
  "push rax\npop rax",
  "push rbx\npop rbx",
  "push rcx\npop rcx",
  "push rdx\npop rdx",
  "nop",
  "xchg rax, rax",
  "xchg rbx, rbx",
  "xchg rcx, rcx",
  "xchg rdx, rdx",
  "xor rax, 0",
  "xor rbx, 0",
  "xor rcx, 0",
  "xor rdx, 0",
]

def random_int
  return "0x#{ '%x' % rand(0..0xFFFFFFFF) }"
end

def nop
  s = []
  s.push("; Nops")
  0.upto(rand(0..2)) do
    s.push(NOPS.sample)
  end
  s.push("")

  return s.join("\n")
end

def generate_asm()
  s = []
  s.push("bits 64")
  s.push("")
  s.push("; Set the seed");
  s.push("mov ecx, #{ random_int() }")

  s.push("")
  s.push("; Start of actual code...");
  s.push("")
  s.push("; checksum(start - rdi, end - rsi")
  s.push("")
  #s.push(nop())
  s.push("top:")
  #          start end
  s.push("cmp rdi, rsi")
  s.push("jge done")

  # Read the current dword
  s.push("mov eax, dword [rdi]")

  # Go to the next dword
  s.push("add rdi, 4")

  0.upto(rand(1..5)) do
    0.upto(rand(2..5)) do
      # Do something to the value a few times
      s.push([
        "xor eax, #{ random_int }",
        "add eax, #{ random_int }",
        "sub eax, #{ random_int }",
        "ror eax, #{ rand(1..30) }",
        "rol eax, #{ rand(1..30) }",
      ].sample)
    end

    # Mix in the previous value (or seed)
    s.push("xor ecx, eax")
    s.push('')
    s.push(nop())
  end
  s.push("jmp top")

  s.push("done:")
  s.push("mov eax, ecx")
  s.push("ret")

  return s.join("\n")
end

def generate_challenge()
  asm_file = Tempfile.new('secure-asset-manager-asm')
  bin_file = Tempfile.new('secure-asset-manager-bin')

  begin
    # Write the assembly and close
    asm_file.write(generate_asm())
    asm_file.close()

    # Just close the bin file - we'll write it from nasm
    bin_file.close()

    # Generate the assembly
    if !system("nasm -o \"#{ bin_file.path }\" \"#{ asm_file.path }\"")
      raise StandardError.new("Failed to assemble our code!")
    end


    # Get the result
    bin_file
  ensure
    asm_file.close()
    asm_file.unlink()
  end
end
