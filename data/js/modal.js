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
let nbOfFiles = 0;

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

  if (files.length && files.length < 4) {
    console.log("File(s) dropped");
    uploadFiles(files);
  } else alert("Too many files");
});

fileInput.addEventListener("change", (event) => {
  const files = event.target.files;
  uploadFiles(files);
});

const uploadFiles = (files) => {
  [...files].forEach((file) => {
    formData.append("file", file);
    createFileDetails(file, nbOfFiles);
    nbOfFiles++;
  });
};

const removeFile = (event) => {
  console.log(formData.getAll("file"));
  console.log(event.currentTarget.id);
	let index =

  //fileDiv.remove();
};

const createFileDetails = (file, nb) => {
  let temp = document.getElementById("clone-template");
  let fileDetails = temp.content.cloneNode(true);

  let fileDiv = fileDetails.getElementById("fileDetails");
  fileDiv.id = "fileDetails_" + nb;
  let fileNameZone = fileDetails.getElementById("fileName");
  fileNameZone.id = "fileName_" + nb;
  let fileSizeZone = fileDetails.getElementById("fileSize");
  fileSizeZone.id = "fileSize_" + nb;
  let cross = fileDetails.getElementById("fileCross");
  cross.id = "fileCross_" + nb;

  cross.addEventListener("click", (event) => {
	event.stopPropagation();
    removeFile(event);
  });
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

  request.upload.addEventListener("progress", (event) => {
    console.log(event);
    if (event.lengthComputable) {
      const pourcentage = (event.loaded / event.total) * 100;
      console.log(`Progression de l'upload : ${pourcentage}%`);

      // Mettre à jour la barre de progression avec 'pourcentage'
    }
  });

  request.send(formData);
};

sendBtn.addEventListener("click", sendRequest);
