/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upload.js                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 15:17:23 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/18 10:01:24 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

var formData = new FormData();
const modal = document.querySelector("#modal");
const dropZone = document.getElementById("drop-zone");
const fileInput = document.getElementById("chooseFile");
const sendBtn = document.getElementById("import-btn");
const notifElem = document.getElementById("notification");
const progressBar = document.getElementById("progress-bar");
let dragCounter = 0;
let nbOfFiles = 0;

/*
 ** Handles the upload file modal logic
 */

const toggleModal = () => {
  modal.classList.remove("pointer-events-none");
  if (modal.classList.contains("hidden")) modal.classList.remove("hidden");
  else {
    modal.classList.add("hidden");
    clearModal();
  }
};

const clearModal = () => {
  let files = formData.getAll("file");
  for (let i = 0; i < files.length; i++) {
    document.getElementById(`fileDetails_${i}`).remove();
  }
  progressBar.classList.add("hidden");
  formData = new FormData();
  nbOfFiles = 0;
};

/*
 ** Handles the upload file logic
 */

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

  if (files.length && files.length <= 5) {
    uploadFiles(files);
  } else showNotification("Too many files. Max. 5.");
});

fileInput.addEventListener("change", (event) => {
  const files = event.target.files;
  if (files.length && files.length <= 5) {
    uploadFiles(files);
    event.currentTarget.value = null;
  } else showNotification("Too many files. Max. 5.");
});

const uploadFiles = (files) => {
  let existingFiles = [];
  [...files].forEach((file) => {
    if (formData.has("file")) {
      for (let [key, val] of formData.entries()) {
        if (file.name === val.name && file.size === val.size) {
          existingFiles.push(file.name);
        }
      }
    }
    if (existingFiles.length) {
      let errorMsg;
      errorMsg = generateErrorMessage(existingFiles);
      showNotification(errorMsg);
      return;
    }
    let currentFiles = formData.getAll("file");
    if (currentFiles.length < 5) {
      formData.append("file", file);
      createFileDetails(file, nbOfFiles);
      nbOfFiles++;
    } else showNotification("Too many files. Max. 5.");
  });
};

const generateErrorMessage = (fileNames) => {
  if (fileNames.length === 1) {
    return `The file \`${fileNames[0]}\` already exists.`;
  } else {
    const lastFile = fileNames.pop();
    return `The files \`${fileNames.join(
      "`, `"
    )}\` and \`${lastFile}\` already exist.`;
  }
};

const showNotification = (errorMsg) => {
  notifElem.getElementsByTagName("span")[0].innerHTML = errorMsg;
  notifElem.classList.remove("opacity-0");
  notifElem.classList.add("opacity-100", "translate-y-0");

  setTimeout(() => {
    notifElem.classList.add("opacity-0");
    notifElem.classList.remove("opacity-100", "translate-y-0");
  }, 3000);
};

const removeFile = (event) => {
  let index = event.currentTarget.id.split("_")[1];
  let files = formData.getAll("file");

  const newFormData = new FormData();
  files.forEach((file, i) => {
    if (i != index) {
      newFormData.append("file", file);
    }
  });
  nbOfFiles--;
  formData = newFormData;

  const fragmentToRemove = document.getElementById("fileDetails_" + index);
  if (fragmentToRemove) fragmentToRemove.remove();
};

const createFileDetails = (file, nb) => {
  let temp = document.getElementById("clone-template");
  let fileDetails = temp.content.cloneNode(true);
  let fileDiv = fileDetails.getElementById("fileDetails");
  let fileNameZone = fileDetails.getElementById("fileName");
  let fileSizeZone = fileDetails.getElementById("fileSize");
  let cross = fileDetails.getElementById("fileCross");

  fileDiv.id = "fileDetails_" + nb;
  fileNameZone.id = "fileName_" + nb;
  fileSizeZone.id = "fileSize_" + nb;
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
  const API_ENDPOINT = "https://file.io"; // http://localhost:80/data/ressources/uploads/
  request.open("POST", API_ENDPOINT, true);
  request.onreadystatechange = () => {
    if (request.readyState === 4 && request.status === 200) {
      console.log(request.responseText);
    }
  };
  return request;
};

const sendRequest = () => {
  let files = formData.getAll("file");
  if (files.length > 0) {
    const request = createRequest();

    request.upload.addEventListener("progress", (event) => {
      if (event.lengthComputable) {
        let pourcentage = ((event.loaded / event.total) * 100).toFixed(1);
        let done = "";
        progressBar.classList.remove("hidden");
        const loading = document.getElementById("loading");
        loading.style.width = `${pourcentage}%`;
        if (pourcentage == "100.0") {
          pourcentage = pourcentage.split(".")[0];
          done = "done";
        }
        progressBar.getElementsByTagName(
          "span"
        )[0].innerHTML = `${pourcentage} % ${done}`;
      }
    });

    modal.classList.add("pointer-events-none");
    setTimeout(() => {
      clearModal();
      toggleModal();
    }, 1750);

    request.send(formData);
  } else {
    showNotification("Please provide at least one file to upload.");
  }
};

sendBtn.addEventListener("click", sendRequest);
