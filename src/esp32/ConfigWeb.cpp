/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#if defined(ARDUINO_ARCH_ESP32)

#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include "ConfigWeb.h"
#include "Settings.h"

const char *htmlPrefix = R"(
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
body{font-family:Arial,sans-serif;background:#f5f5f5;padding:5px;margin:0}
.c{max-width:800px;margin:0 auto;background:#fff;border-radius:8px;padding:12px;box-shadow:0 2px 4px rgba(0,0,0,.1)}
h2{font-size:15px;margin:12px 0 6px;border-bottom:2px solid #3498db;padding-bottom:2px}
table{width:100%;border-collapse:collapse}
td{padding:2px 6px}
td:first-child{width:35%;font-weight:500}
input[type="text"]{width:100%;padding:4px;border:1px solid #ddd;border-radius:4px;font-size:14px}
input[type="text"]:focus{outline:none;border-color:#3498db}
.btn{background:#3498db;color:#fff;border:none;padding:6px 16px;border-radius:4px;cursor:pointer;font-size:14px}
.btn:hover{background:#2980b9}
.btn:disabled{background:#95a5a6;cursor:not-allowed}
.s{text-align:center;padding-top:8px}
.modal{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,.5);z-index:1000}
.modal-content{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);background:#fff;padding:20px;border-radius:8px;box-shadow:0 4px 8px rgba(0,0,0,.2);min-width:250px;text-align:center}
.modal-content h3{margin:0 0 12px;font-size:16px;color:#333}
.modal-content .btn{margin-top:12px;margin-left:8px}
</style>
</head>
<body>
<div class="c">
<h2>RCC Configuration</h2>
<form method="POST" action="/submit">
<table>
)";

const char *htmlInput = R"(
<tr><td>$user_readable$</td><td><input type="text" name="$key$" value="$value$"></td></tr>
)";

const char *htmlSuffix = R"raw(
</table>
<div class="s"><input type="submit" value="Save Settings" class="btn"></div>
</form>
<h2>Sound Upload</h2>
<form method="POST" action="/upload" enctype="multipart/form-data" id="f">
<input type="file" name="sounds" accept=".wav" multiple id="i" style="display:none">
<label for="i" class="btn">Choose</label>
<button type="submit" class="btn" id="b">Upload</button>
<span id="s" style="color:#666;font-size:14px">No file</span>
</form>
<h2>Firmware Update</h2>
<form method="POST" action="/firmware" enctype="multipart/form-data" id="fw">
<input type="file" name="firmware" accept=".bin" id="fwi" style="display:none">
<label for="fwi" class="btn">Choose</label>
<button type="submit" class="btn" id="fwb">Update</button>
<span id="fws" style="color:#666;font-size:14px">No file</span>
</form>
<div id="modal" class="modal"><div class="modal-content"><h3 id="modal-msg"></h3><div id="modal-btns"><button class="btn" onclick="closeModal()">OK</button></div></div></div>
<script>
var confirmCallback=null;
function showModal(msg,isConfirm,callback){document.getElementById("modal-msg").textContent=msg;var btns=document.getElementById("modal-btns");if(isConfirm){confirmCallback=callback;btns.innerHTML='<button class="btn" onclick="confirmYes()">Yes</button><button class="btn" onclick="closeModal()">No</button>'}else{btns.innerHTML='<button class="btn" onclick="closeModal()">OK</button>'}document.getElementById("modal").style.display="block"}
function closeModal(){document.getElementById("modal").style.display="none";confirmCallback=null}
function confirmYes(){var cb=confirmCallback;closeModal();if(cb)cb()}
var i=document.getElementById("i"),s=document.getElementById("s"),b=document.getElementById("b");
var fwi=document.getElementById("fwi"),fws=document.getElementById("fws"),fwb=document.getElementById("fwb");
i.onchange=function(){s.textContent=this.files.length?this.files.length==1?this.files[0].name:this.files.length+" files":"No file";s.style.color=this.files.length?"#333":"#666"};
fwi.onchange=function(){fws.textContent=this.files.length?this.files[0].name:"No file";fws.style.color=this.files.length?"#333":"#666"};
document.querySelector("form[action=\"/submit\"]").onsubmit=function(e){e.preventDefault();var d=new FormData(this);fetch("/submit",{method:"POST",body:d}).then(function(r){return r.text()}).then(function(){showModal("Settings saved successfully",false);setTimeout(function(){location.reload()},1500)}).catch(function(){showModal("Failed to save settings",false)})};
document.getElementById("f").onsubmit=function(e){e.preventDefault();if(!i.files.length){showModal("Please select a file",false);return}b.disabled=true;s.textContent="Uploading...";s.style.color="#3498db";var d=new FormData(this);fetch("/upload",{method:"POST",body:d}).then(function(r){if(r.ok)return r.text();throw new Error("Upload failed")}).then(function(){showModal("Upload successful",false);b.disabled=false;s.textContent="No file";s.style.color="#666";i.value=""}).catch(function(e){showModal("Upload failed",false);b.disabled=false;s.textContent="No file";s.style.color="#666";i.value=""})};
document.getElementById("fw").onsubmit=function(e){e.preventDefault();if(!fwi.files.length){showModal("Please select a firmware file",false);return}showModal("Update firmware? Device will reboot.",true,function(){fwb.disabled=true;fws.textContent="Updating...";fws.style.color="#3498db";var d=new FormData(document.getElementById("fw"));fetch("/firmware",{method:"POST",body:d}).then(function(r){if(r.ok)return r.text();throw new Error("Update failed")}).then(function(){showModal("Firmware updated! Device rebooting...",false);setTimeout(function(){location.reload()},10000)}).catch(function(e){showModal("Firmware update failed",false);fwb.disabled=false;fws.textContent="No file";fws.style.color="#666";fwi.value=""})})};
</script>
</div>
</body>
</html>
)raw";

const char *htmlSubmitted = "OK";

const char *htmlUploadSuccess = "OK";

const char *htmlUploadError = "ERROR";

WebServer ConfigWeb::server(80);

void ConfigWeb::handleRoot()
{
    String form = String(htmlPrefix);
    String name = storage.openFirst(SETTINGS_PATH);
    while (!name.isEmpty()) {
        String inputbox = String(htmlInput);
        inputbox.replace("$user_readable$", name);
        inputbox.replace("$key$", name);
        inputbox.replace("$value$", settings.get(name.c_str()));
        form += inputbox;
        name = storage.openNext();
    }
    form += htmlSuffix;
    server.send(200, "text/html", form);
}

void ConfigWeb::handleSubmit()
{
    for (int i = 0; i < server.args(); i++) {
        String name = server.argName(i);
        settings.set(server.argName(i).c_str(), server.arg(i));
    }
    server.send(200, "text/plain", htmlSubmitted);
}

// Static variables to track upload state
static String uploadFilename;
static String uploadFilepath;
static bool uploadValid = true;
static String uploadError;
static size_t uploadTotalWritten = 0;
static bool uploadHeaderSkipped = false;

void ConfigWeb::handleUpload()
{
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        uploadFilename = upload.filename;
        
        // Remove .wav extension
        if (uploadFilename.endsWith(".wav")) {
            uploadFilename = uploadFilename.substring(0, uploadFilename.length() - 4);
        }
        
        Serial.print("[Web] Upload start: ");
        Serial.println(uploadFilename);
        
        uploadValid = true;
        uploadError = "";
        uploadTotalWritten = 0;
        uploadHeaderSkipped = false;
        uploadFilepath = storage.addFolder(SOUNDS_PATH, uploadFilename.c_str());
        
        // Check WAV header when first chunk arrives
        if (upload.currentSize >= 44) {
            // Validate WAV header
            if (upload.buf[0] != 'R' || upload.buf[1] != 'I' || 
                upload.buf[2] != 'F' || upload.buf[3] != 'F') {
                uploadValid = false;
                uploadError = "Not a valid WAV file";
                return;
            }
            
            // Check format (should be PCM = 1)
            uint16_t audioFormat = upload.buf[20] | (upload.buf[21] << 8);
            if (audioFormat != 1) {
                uploadValid = false;
                uploadError = "Only PCM format supported";
                return;
            }
            
            // Check sample rate (should be 16000 Hz)
            uint32_t sampleRate = upload.buf[24] | (upload.buf[25] << 8) | 
                                  (upload.buf[26] << 16) | (upload.buf[27] << 24);
            if (sampleRate != 16000) {
                uploadValid = false;
                uploadError = "Sample rate must be 16000 Hz, got " + String(sampleRate) + " Hz";
                return;
            }
            
            // Check bits per sample (should be 16)
            uint16_t bitsPerSample = upload.buf[34] | (upload.buf[35] << 8);
            if (bitsPerSample != 16) {
                uploadValid = false;
                uploadError = "Must be 16-bit, got " + String(bitsPerSample) + "-bit";
                return;
            }
        }
        
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (uploadValid) {
            // Skip WAV header (44 bytes), write only PCM data
            if (!uploadHeaderSkipped) {
                if (uploadTotalWritten + upload.currentSize >= 44) {
                    size_t skip = 44 - uploadTotalWritten;
                    storage.write(uploadFilepath.c_str(), upload.buf + skip, upload.currentSize - skip, uploadTotalWritten > 0);
                    uploadHeaderSkipped = true;
                }
                uploadTotalWritten += upload.currentSize;
            } else {
                storage.append(uploadFilepath.c_str(), upload.buf, upload.currentSize);
            }
        }
        
    } else if (upload.status == UPLOAD_FILE_END) {
        if (uploadValid) {
            Serial.print("[Web] Upload complete: ");
            Serial.println(uploadFilename);
        } else {
            Serial.print("[Web] Upload failed: ");
            Serial.println(uploadError);
        }
    }
}

void ConfigWeb::handleUploadComplete()
{
    if (uploadValid) {
        server.send(200, "text/plain", htmlUploadSuccess);
    } else {
        server.send(400, "text/plain", htmlUploadError);
    }
}

void ConfigWeb::handleFirmwareUpload()
{
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[Web] Firmware update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace, U_FLASH)) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("[Web] Update Success: %u bytes\nRebooting...\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
    }
}

void ConfigWeb::handleFirmwareComplete()
{
    if (Update.hasError()) {
        server.send(400, "text/plain", "ERROR");
    } else {
        server.send(200, "text/plain", "OK");
        delay(100);
        ESP.restart();
    }
}

void ConfigWeb::begin()
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/submit", HTTP_POST, handleSubmit);
    server.on("/upload", HTTP_POST, handleUploadComplete, handleUpload);
    server.on("/firmware", HTTP_POST, handleFirmwareComplete, handleFirmwareUpload);

    server.begin();
    Serial.println("[Web] Server started");
}
#endif