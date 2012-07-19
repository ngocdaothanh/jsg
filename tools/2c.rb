# Usage: ruby 2c.rb <directory1> [directory2] [directory3]

$file_names = []
for d in ARGV
  not_dirs = Dir::glob("#{d}/**/*").select { |fn| File.file?(fn) }
  $file_names.concat(not_dirs)
end

def to_hex_array(string)
  ret = []
  string.each_byte do |byte|
    hex  = byte.to_s(16)
    hex2 = if hex.length == 1 then "0#{hex}" else hex end
    ret << "0x#{hex2}"
  end
  ret
end

#------------------------------------------------------------------------------
# main

$file_fun_hash = {}
$file_names.each do |file_name|
  replaced = file_name
  [File::Separator, '.', '-'].each do |c|
    replaced = replaced.gsub(c, '_')
  end
  fun_name = "jsg_assets_2c_load_#{replaced}"
  $file_fun_hash[file_name] = fun_name
end

puts "#ifndef __JSG_ASSETS_2C_H__"
puts "#define __JSG_ASSETS_2C_H__\n\n"
puts '#include <string.h>'

$file_fun_hash.each do |file_name, fun_name|
  puts "static void #{fun_name}(unsigned char** bytes, int* len) {"

  string = File.read(file_name)
  array  = to_hex_array(string)

  puts "  unsigned char x[] = {#{array.join(', ')}};"

  len = array.length
  puts "  *len   = #{len};"
  puts "  *bytes = (unsigned char*) malloc(#{len});"
  puts "  memcpy(*bytes, x, #{len});"
  puts "}\n\n"
end

puts "void jsg_assets_2c_load(const char* file_name, unsigned char** bytes, int* len) {"
$file_fun_hash.each do |file_name, fun_name|
  puts "  if (strcmp(file_name, \"#{file_name}\") == 0) { #{fun_name}(bytes, len); return; }"
end
puts "\n"
puts "  *bytes = NULL;"
puts "  *len   = 0;"
puts "}"
puts '#endif'
