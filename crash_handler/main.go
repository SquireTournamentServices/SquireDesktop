package main

import (
	//"fmt"
	//"io/ioutil"
	"log"
	"os"
	"os/exec"
)

const MAX_BUFFER = 2000
const MAX_LINES = 200
const VERSION_INFO_LINES = 5

func main() {
	exec_cmd := "SquireDesktop"
	post_url := "https://discord.com/api/webhooks/1027978424687538287/RFrXdnRWcAB1Oa4NFyNtebvlZSn7VE2pIOab0NVDBxJqZnFdzjPGZecyLFObljVsg5ul"

	args := os.Args
	if len(args) == 3 {
		exec_cmd = args[1]
		post_url = args[2]
	} else {
		log.Printf("Usage %s <command> <post url>", args[0])
		log.Println("Defaulting to default settings")
	}

	log.Printf("Crash reporter linked to %s and, sends to %s", exec_cmd, post_url)
	cmd := exec.Command(exec_cmd)

	//stdout, _ := cmd.StdoutPipe()
	cmd.Start()

	// Keep a buffer of MAXbytes of program output
	cmd.Wait()

	// Ask for confirmation then, send the crash report
}
