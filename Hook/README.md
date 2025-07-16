# Jafg Hook Tool

This tool is a hook post-push system for git. It runs after a push to the git master repository was done.
It is meant to update binaries or other meta-data automatically.

# Getting up and running

1. Get the GitHub Secret from the repository settings and set it in the `.env` file as `SECRET_X_HUB_SIGNATURE`.
2. Run the following command to build the Docker image:
   ```bash
   docker build -t jafg-hook .
   ```
3.
   ```bash
   docker run -d --name jafg-hook -p 8000:8000 --env-file .env jafg-hook
   ```
