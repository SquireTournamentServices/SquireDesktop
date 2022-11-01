package main

import (
	"bytes"
	"encoding/json"
	"net/http"
)

const NAME = "Squire Desktop Error Reporter"

type DiscordWebhookFields struct {
	Name   string `json:"name"`
	Value  string `json:"value"`
	Inline bool   `json:"inline,omitempty"`
}

type DiscsordWebhookEmbed struct {
	Title       string                 `json:"title,omitempty"`
	Description string                 `json:"description,omitempty"`
	Url         string                 `json:"url,omitempty"`
	Fields      []DiscordWebhookFields `json:"fields,omitempty"`
}

type DiscordWebhookBase struct {
	Content string                 `json:"content"`
	Embeds  []DiscsordWebhookEmbed `json:"embeds,omitempty"`
	Tts     bool                   `json:"tts,omitempty"`
}

func SendWebhook(hook DiscordWebhookBase, url string) error {
	enc, err := json.Marshal(hook)
	if err != nil {
		return err
	}

	reader := bytes.NewReader(enc)
	resp, err := http.Post(url, "application/json", reader)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	return nil
}
