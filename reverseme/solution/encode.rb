loop do
  b = STDIN.read(1)
  if(b.nil?)
    exit(0)
  end

  print (b.ord ^ 0x41).chr
end
