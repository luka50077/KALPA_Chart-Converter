package main

import (
	"context"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"net/http"
	"os"

	"golang.org/x/oauth2"
	"golang.org/x/oauth2/google"
	"google.golang.org/api/drive/v3"
	"google.golang.org/api/option"
)


func getClient(config *oauth2.Config) *http.Client {
	listener, err := net.Listen("tcp", "localhost:0")
	if err != nil {
		log.Fatalf("Failed to listen on local port: %v", err)
	}
	redirectURL := "http://" + listener.Addr().String()
	config.RedirectURL = redirectURL

	authURL := config.AuthCodeURL("state-token",
		oauth2.AccessTypeOffline,
		oauth2.ApprovalForce,
	)

	fmt.Printf("Open this URL in your browser:\n%s\n", authURL)

	ch := make(chan string)

	go func() {
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			code := r.URL.Query().Get("code")
			fmt.Fprintf(w, "Authorization complete. You can close this window.")
			ch <- code
		})
		http.Serve(listener, nil)
	}()

	code := <-ch

	tok, err := config.Exchange(context.Background(), code)
	if err != nil {
		log.Fatalf("Unable to retrieve token: %v", err)
	}

	f, err := os.Create("../hidden/token.json")
	if err != nil {
		log.Fatalf("Unable to save token: %v", err)
	}
	json.NewEncoder(f).Encode(tok)

	return config.Client(context.Background(), tok)
}

func main() {
	uploadFile := "no_path.txt"
	uploadFilePath := "no_path.txt"
	if len(os.Args) != 2 {
		fmt.Println("Usage: go run uploader.go <upload_file_name>")
		return
	}else{
		uploadFile = os.Args[1]
		uploadFilePath =  "../../encode/" + os.Args[1]
	}


	ctx := context.Background()

	b, err := os.ReadFile("../hidden/credentials.json")
	if err != nil {
		log.Fatalf("Unable to read credentials.json: %v", err)
	}

	config, err := google.ConfigFromJSON(b, drive.DriveFileScope)
	if err != nil {
		log.Fatalf("Unable to parse config: %v", err)
	}

	client := getClient(config)

	srv, err := drive.NewService(ctx, option.WithHTTPClient(client))
	if err != nil {
		log.Fatalf("Unable to create Drive client: %v", err)
	}

	f, err := os.Open(uploadFilePath)
	if err != nil {
		log.Fatalf("Unable to open local file: %v", err)
	}
	defer f.Close()

	// upload to KALPA_ChartData_Encoded folder
	// Load folder ID from hidden JSON
	folderFile, err := os.Open("../hidden/folder_path.json")
	if err != nil {
		log.Fatalf("Cannot open folder_path.json: %v", err)
	}
	defer folderFile.Close()

	var folderData struct {
		UploadFolderID string `json:"upload_folderID"`
	}

	if err := json.NewDecoder(folderFile).Decode(&folderData); err != nil {
		log.Fatalf("Cannot parse folder_path.json: %v", err)
	}
	folderID := folderData.UploadFolderID
	driveFile := &drive.File{Name: uploadFile, Parents: []string{folderID}}

	res, err := srv.Files.Create(driveFile).Media(f).Do()
	if err != nil {
		log.Fatalf("Unable to upload file: %v", err)
	}

	fmt.Printf("Uploaded! File ID = %s\n", res.Id)
}
