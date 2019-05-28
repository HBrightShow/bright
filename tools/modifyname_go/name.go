package main

import (
	"fmt"
	"os"
	"flag"
	"path/filepath"
	"strings"
	"io/ioutil"
	"bytes"
)


func main() {

	var dir string
	flag.StringVar(&dir, "d", "", "directory path")

	flag.Parse()

	if dir == "" {
		flag.Usage()
		fmt.Println("no directory path.")
		return 
	}

	var nameMap map[string]string
	nameMap = make(map[string]string)
	paths := make([]string, 0)

	nameMap["eos"] = "uos"
	nameMap["EOS"] = "UOS"
	nameMap["Eos"] = "Uos"

	filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		paths = append(paths, path)
		return nil
	})
	
	modifyFileContent(paths, nameMap)
	modifyFileName(paths, nameMap)

	fmt.Println("finish!")
}

func modifyFileContent( paths []string, name map[string]string ) {
	
	const maxFileSize uint64 = 50 * 1024 *1024

	for _,path := range paths {
		dir, _ := os.Stat(path)
		if dir.IsDir() {
			continue
		}

		f, err := os.OpenFile(path, os.O_RDWR, 0666)
		//defer f.Close()
		if err != nil {
			fmt.Println("ERROR:: OpenFile failed! path: ", path)
			f.Close()
			continue
		}

		fileSize, _ := f.Seek(0, os.SEEK_END)
		f.Seek(0, os.SEEK_SET)

		if uint64(fileSize) > maxFileSize {
			fmt.Println("ERROR:: filesize too large! path: ", path)
			f.Close()
			continue
		}

		srcBytes, err := ioutil.ReadAll(f)

		if err != nil {
			fmt.Println("ERROR:: ReadAll failed! path: ", path)
			f.Close()
			continue
		}
		
		f.Close()

		for oldName,newName := range name {
			var oldbyte []byte = []byte(oldName)
			var newByte []byte = []byte(newName)
			srcBytes = bytes.Replace(srcBytes, oldbyte, newByte, -1)
		}

		err = ioutil.WriteFile(path, srcBytes, 0666)
		if err != nil {
			fmt.Println("ERROR::", err, " path: ", path)
			continue
		}
	}
} 

func modifyFileName( paths []string, name map[string]string ) {

	var newName, oldName, newPath, strold string
	var nCount uint64

	for i := len(paths) - 1; i >= 0; i-- {
		nCount++

		oldName = paths[i]
		pos := strings.LastIndex(oldName, "/")
		oldName = paths[i][pos + 1:]

		strold = oldName
		for k, v := range name {
			newName = strings.Replace(strold, k, v, -1)

			if newName != strold {
				strold = newName
			}
		}

		if newName != oldName {
			fmt.Println("BEGIN: ", paths[i])

			newPath = paths[i][:pos + 1] + newName
			err := os.Rename(paths[i], newPath)
			if err != nil {
				fmt.Printf("%s", err)
			}
		}	
	}

	fmt.Println("nCount: ", nCount)
}

