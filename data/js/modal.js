/*
 ** Handles the upload file modal logic
 */

const toggleModal = () => {
  const modal = document.querySelector("#modal");
  if (modal.classList.contains("hidden")) modal.classList.remove("hidden");
  else modal.classList.add("hidden");
};

/*
 ** Handles the upload file logic
 */

const formData = new FormData();
const dropZone = document.getElementById("drop-zone");
const fileInput = document.getElementById("chooseFile");
const sendBtn = document.getElementById("import-btn");
let dragCounter = 0;

dropZone.addEventListener("dragenter", (event) => {
  event.preventDefault();
  dragCounter++;
  dropZone.classList.add("border-teal-600");
  dropZone.classList.remove("border-gray-200");
});

dropZone.addEventListener("dragover", (event) => {
  event.preventDefault();
});

dropZone.addEventListener("dragleave", () => {
  dragCounter--;
  if (dragCounter === 0) {
    dropZone.classList.remove("border-teal-600");
    dropZone.classList.add("border-gray-200");
  }
});

dropZone.addEventListener("drop", (event) => {
  event.preventDefault();
  dragCounter = 0;
  dropZone.classList.remove("border-teal-600");
  dropZone.classList.add("border-gray-200");

  const files = event.dataTransfer.files;

  if (files.length && files.length < 3) {
    console.log("File(s) dropped");
    uploadFiles(files);
  } else alert("Too many files");
});

fileInput.addEventListener("change", (event) => {
  const files = event.target.files;
  uploadFiles(files);
});

const uploadFiles = (files) => {
  [...files].forEach((file, i) => {
    formData.append("file", file);
    createFileDetails(file);
  });
  console.log(formData.getAll("file"));
};

const createFileDetails = (file) => {
  let temp = document.getElementById("dropped-file-details");
  let fileDetails = temp.content.cloneNode(true);
  let fileNameZone = fileDetails.getElementById("fileName");
  let fileSizeZone = fileDetails.getElementById("fileSize");

  let cross = fileDetails.getElementById("file-cross");

  const fileSizeInKb = (file.size / 1000).toFixed(2);
  fileSizeZone.innerHTML = `${fileSizeInKb} kB`;

  fileNameZone.innerHTML = file.name;

  dropZone.parentNode.insertBefore(fileDetails, dropZone.nextSibling);
};

const createRequest = () => {
  const request = new XMLHttpRequest();
  const API_ENDPOINT = "https://file.io";
  request.open("POST", API_ENDPOINT, true);
  request.onreadystatechange = () => {
    if (request.readyState === 4 && request.status === 200) {
      console.log(request.responseText);
    }
  };
  return request;
};

const sendRequest = () => {
  const request = createRequest();
  request.send(formData);

  request.upload.addEventListener("progress", (event) => {
    if (event.lengthComputable) {
      const pourcentage = (event.loaded / event.total) * 100;
      console.log(pourcentage);
      // Mettre à jour la barre de progression avec 'pourcentage'
    }
  });
};

sendBtn.addEventListener("click", sendRequest);
