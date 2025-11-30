package main

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
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
		log.Fatalf("Failed to listen: %v", err)
	}

	config.RedirectURL = "http://" + listener.Addr().String()

	authURL := config.AuthCodeURL("state-token",
		oauth2.AccessTypeOffline,
		oauth2.ApprovalForce,
	)

	fmt.Printf("Open this URL in your browser:\n%s\n", authURL)

	ch := make(chan string)

	go func() {
		http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			code := r.URL.Query().Get("code")
			fmt.Fprintf(w, "Authorization completed. Close this window.")
			ch <- code
		})
		http.Serve(listener, nil)
	}()

	code := <-ch

	tok, err := config.Exchange(context.Background(), code)
	if err != nil {
		log.Fatalf("Token exchange failed: %v", err)
	}

	f, _ := os.Create("../hidden/token.json")
	json.NewEncoder(f).Encode(tok)

	return config.Client(context.Background(), tok)
}

func fileExists(path string) bool {
	_, err := os.Stat(path)
	return err == nil
}

func main() {
	// Load folder ID from hidden JSON
	f, err := os.Open("../hidden/folder_path.json")
	if err != nil {
		log.Fatalf("Cannot open folder_path.json: %v", err)
	}
	defer f.Close()

	var folderData struct {
		DownloadFolderID string `json:"download_folderID"`
	}

	if err := json.NewDecoder(f).Decode(&folderData); err != nil {
		log.Fatalf("Cannot parse folder_path.json: %v", err)
	}
	folderID := folderData.DownloadFolderID

	saveDir := "../../videos"

	os.MkdirAll(saveDir, 0755)

	ctx := context.Background()

	b, err := os.ReadFile("../hidden/credentials.json")
	if err != nil {
		log.Fatalf("Unable to read credentials.json: %v", err)
	}

	config, err := google.ConfigFromJSON(b, drive.DriveReadonlyScope)
	if err != nil {
		log.Fatalf("Config parse error: %v", err)
	}

	client := getClient(config)

	srv, err := drive.NewService(ctx, option.WithHTTPClient(client))
	if err != nil {
		log.Fatalf("Drive service creation failed: %v", err)
	}


	query := fmt.Sprintf("'%s' in parents and trashed = false", folderID)

	r, err := srv.Files.List().
		Q(query).
		Fields("files(id, name, mimeType)").
		PageSize(1000).
		Do()

	if err != nil {
		log.Fatalf("Folder list error: %v", err)
	}

	if len(r.Files) == 0 {
		fmt.Println("No files found in folder.")
		return
	}

	fmt.Printf("Files in folder: %d\n\n", len(r.Files))


	for _, f := range r.Files {

		savePath := saveDir + "/" + f.Name

		if fileExists(savePath) {
			fmt.Printf("[SKIP ] %s (already exists)\n", savePath)
			continue
		}

		if f.MimeType == "application/vnd.google-apps.folder" {
			fmt.Printf("[SKIP ] %s is a folder\n", f.Name)
			continue
		}

		res, err := srv.Files.Get(f.Id).Download()
		if err != nil {
			fmt.Printf("[ERROR] Cannot download %s: %v\n", f.Name, err)
			continue
		}
		defer res.Body.Close()

		out, err := os.Create(savePath)
		if err != nil {
			fmt.Printf("[ERROR] Cannot create %s: %v\n", savePath, err)
			continue
		}

		_, err = io.Copy(out, res.Body)
		out.Close()

		if err != nil {
			fmt.Printf("[ERROR] Failed writing %s: %v\n", savePath, err)
			continue
		}

		fmt.Printf("[OK] %s\n", savePath)
	}

	fmt.Println("\nDownload finished.")
}
