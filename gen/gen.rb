#!/usr/bin/env ruby

require "json"
require "pathname"

input_path = Pathname.new ARGV[0]
output_path = Pathname.new ARGV[1]

if input_path.empty?
    puts "first argument must point to the input path"
    exit 1
end

if output_path.empty?
    puts "second argument must point to the output path"
    exit 1
end

class Parameter
    attr_accessor :name, :type

    def initialize(attributes)
        if attributes["name"].nil? || !attributes["name"].instance_of?(String)
            puts "parameter name isn't a string: #{attributes["name"]}"
            exit 1
        end

        if attributes["type"].nil? || !attributes["type"].instance_of?(String)
            puts "parameter type isn't a string: #{attributes["type"]}"
            exit 1
        end

        @name = attributes["name"]
        @type = attributes["type"]
    end
end

class FunctionVersion
    attr_accessor :return_type, :parameters, :flags

    def initialize(attributes)
        if attributes["return_type"].nil? || !attributes["return_type"].instance_of?(String)
            puts "function version return_type isn't a string: #{attributes["return_type"]}"
            exit 1
        end

        if attributes["flags"].nil? || attributes["flags"].any? { !_1.instance_of?(String) }
            puts "function version flags weren't all strings: #{attributes["flags"]}"
            exit 1
        end

        if attributes["flags"].length > 64
            puts "WARNING: Cannot specify more than 64 flags"
            exit 1
        end

        @return_type = attributes["return_type"]
        @parameters = attributes["parameters"].map{ Parameter.new _1 }.to_a
        @flags = attributes["flags"]
    end
end

class Function
    attr_accessor :name, :versions

    def initialize(attributes)
        if !attributes["name"].instance_of?(String)
            puts "function name isn't a string: #{attributes["name"]}"
            exit 1
        end

        @name = String.new attributes["name"]
        @versions = attributes["versions"].map{ FunctionVersion.new _1 }.to_a
    end
end

class Type
    attr_accessor :name, :depends_on, :functions

    def initialize(attributes)
        if !attributes["name"].instance_of?(String)
            puts "type name isn't a string: #{attributes["name"]}"
            exit 1
        end

        if attributes["depends_on"].any? { !_1.instance_of?(String) }
            puts "type depends_on weren't all strings: #{attributes["depends_on"]}"
            exit 1
        end

        @name = String.new attributes["name"]
        @depends_on = attributes["depends_on"]
        @functions = attributes["functions"].map{ Function.new _1 }.to_a
    end
end

Dir.chdir("#{input_path}/sources")

Dir.glob("**/*.json").each { |file|
    source_path = Pathname.new file
    source_path = source_path.sub_ext(".h")

    out_path = output_path + source_path

    output = Type.new JSON.parse(File.read(file))

    type = output.name
    type_pascal = type.split("_").map(&:capitalize).join

    File.open(out_path, "w+") { |f|
        f.write("#pragma once\n\n")

        f.write(["stdbool", "stddef", "stdint"].map{"#include <#{_1}.h>"}.join("\n"))

        f.write("\n\n")

        f.write(output.depends_on.map{"#include \"#{_1}.h\""}.join("\n"))

        f.write("\n\n")

        f.write("struct #{type_pascal};\n\n")

        output.functions.each { |func|
            func_name = func.name

            func_name_pascal = func_name.split("_").map(&:capitalize).join

            func.versions.each.with_index(1) { |func_version, version|
                ret_type = func_version.return_type
                args = func_version.parameters.map{ "#{_1.type} #{_1.name}" }.join(", ")

                enum_name = "Ikarus#{type_pascal}#{func_name_pascal}V#{version}Flags"

                flags = func_version.flags

                enum_flags = flags.map.with_index { "    #{enum_name}_#{_1.split("_").map(&:capitalize).join} = 1 << #{_2}" }.join(",\n")

                potential_lf = flags.empty?? "" : "\n"

                f.write("enum #{enum_name} {#{potential_lf}#{enum_flags}#{potential_lf}};\n\n")

                f.write("#{ret_type} ikarus_#{type}_#{func_name}_v#{version}(#{args}, #{enum_name} flags, ErrorCode * err_out);\n\n")
            }
        }
    }
}