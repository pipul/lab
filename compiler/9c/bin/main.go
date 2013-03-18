package main

import (
	"runtime"
)


func main() {
	a1 := []int{4,3,4}
	b1 := len(a1)
	print(runtime.GOOS, b1)
}
