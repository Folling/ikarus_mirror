package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"
	"path/filepath"
	"strings"
)

type ReturnTypeMember struct {
	Name string `json:"name"`
	Type string `json:"type"`
}

type Parameter struct {
	Name string `json:"name"`
	Type string `json:"type"`
}

type FunctionVersion struct {
	ReturnType []ReturnTypeMember `json:"return_type"`
	Parameters []Parameter        `json:"parameters"`
}

type Function struct {
	Name     string            `json:"name"`
	Versions []FunctionVersion `json:"versions"`
}

type Type struct {
	Name           string     `json:"name"`
	GenerateStruct bool       `json:"generate_struct"`
	DependsOn      []string   `json:"depends_on"`
	Functions      []Function `json:"functions"`
}

func pascalize(str string) string {
	builder := strings.Builder{}

	for _, piece := range strings.Split(str, "_") {
		builder.WriteString(strings.ToUpper(piece[0:1]))
		builder.WriteString(piece[1:])
	}

	return builder.String()
}

func main() {
	input_ptr := flag.String("input", "", "the directory in which the JSON sources lie")
	output_ptr := flag.String("output", "", "the directory in which the generated headers will be written")
	flag.Parse()

	input := *input_ptr
	output := *output_ptr

	if len(strings.TrimSpace(input)) == 0 {
		log.Fatal("input path is empty")
	}

	if len(strings.TrimSpace(output)) == 0 {
		log.Fatal("output path is empty")
	}

	err := os.Chdir(fmt.Sprintf("%s", input))

	if err != nil {
		log.Fatalf("unable to change directory to sources dir: %v", err)
	}

	err = filepath.Walk(".", func(path string, info fs.FileInfo, err error) error {
		if err != nil {
			return err
		}

		if info.IsDir() {
			return nil
		}

		content, err := os.ReadFile(path)

		if err != nil {
			log.Fatalf("unable to read file %s: %v", path, err)
		}

		var tp Type
		err = json.Unmarshal([]byte(content), &tp)

		if err != nil {
			log.Fatalf("unable to parse json: %v", err)
		}

		out_path := filepath.Join(output, strings.Replace(path, ".json", ".h", 1))

		f, err := os.OpenFile(out_path, os.O_TRUNC|os.O_WRONLY|os.O_CREATE, 0755)

		if err != nil {
			log.Fatal(err)
		}

		defer f.Close()

		Write := func(str string) {
			_, err := f.WriteString(fmt.Sprintf("%s\n", str))

			if err != nil {
				log.Fatalf("unable to write string to file: %v", err)
			}
		}

		Writef := func(str string, args ...any) {
			Write(fmt.Sprintf(str, args...))
		}

		Write("#pragma once\n")

		for _, include := range []string{"cstdbool", "cstddef", "cstdint"} {
			Writef("#include <%s>", include)
		}

		Write("")

		for _, include := range tp.DependsOn {
			Writef("#include \"%s\"", include)
		}

		Write("")

		if tp.GenerateStruct {
			Writef("struct %s;", pascalize(tp.Name))
		}

		return nil
	})

	if err != nil {
		log.Fatalf("unable to glob JSON files: %v", err)
	}
}
