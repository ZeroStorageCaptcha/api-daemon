# ZSC API daemon
Zero Storage Captcha selfhosted REST API service

## Compile
Qt5 and OpenSSL are required
```
sudo apt install git qt5-default libssl-dev
git clone --recursive https://github.com/ZeroStorageCaptcha/api-daemon.git zero-captcha-daemon
cd zero-captcha-daemon
qmake && make
```

## Run
```
-a --address  Address to bind
-p --port     Port to bind (7697 by default)
-t --threads  Working threads (hardware value by default)
```

## API
GET request, JSON response

**Each response contains a boolean "status" that indicates the logic success of the operation. If !status, read "message" field.**

1. Generate captcha
   ```
   -> /generate?length= [>0] &difficulty= [0-5] &output= [base64|file] IF output=file: &filepath= [file system path]
   <- "token", IF output=base64: "png" (base64 encoded picture)
   ```
2. Validate captcha
   ```
   -> /validate?answer= [user's answer] &token = [user's token]
   <- boolean "valid"
   ```
3. Settings

    3.1 Tokens case sensitive to captcha answer:
    ```
     -> /settings?case_sensitive= [enable|disable]
    ```
    3.2 Only numbers mode:
    ```
    -> /settings?number_mode= [enable|disable]
    ```
## Example
1. Generate
   ```
   Client: /generate?length=4&difficulty=3&output=base64 
   Server: 
   {
     "png": "iVBORw0KGgoAAAANSU <...> AAAAAElFTkSuQmCC",
     "status": true,
     "token":" i2oefBw6mswaORIphgDcY7GwnS"
   }
   ```
2. Validate
   ```
   Client: /validate?answer=iuda1&token=i2oefBw6mswaORIphgDcY7GwnS
   Server: 
   {
     "status": true,
     "valid": false
   }
   ```
