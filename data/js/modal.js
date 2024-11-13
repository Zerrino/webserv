const toggleModal = () => {
  const modal = document.querySelector("#modal");
  if (modal.classList.contains("hidden")) modal.classList.remove("hidden");
  else modal.classList.add("hidden");
};

const dropZone = document.getElementById("drop-zone");
const fileDetails = document.getElementById("dropped-file-details");
const fileNameZone = document.getElementById("fileName");
const fileSizeZone = document.getElementById("fileSize");

dropZone.addEventListener("dragenter", (event) => {
  event.preventDefault();
  dropZone.classList.add("border-teal-600");
  dropZone.classList.remove("border-gray-200");
});

dropZone.addEventListener("dragover", (event) => {
  event.preventDefault();
});

dropZone.addEventListener("dragleave", () => {
  dropZone.classList.remove("border-teal-600");
  dropZone.classList.add("border-gray-200");
});

dropZone.addEventListener("drop", (event) => {
  event.preventDefault();
  dropZone.classList.remove("border-teal-600");
  dropZone.classList.add("border-gray-200");

  const files = event.dataTransfer.files;
  if (files.length) {
    console.log("File(s) dropped");
    [...files].forEach((file, i) => {
      console.log(file);
      console.log(`… file[${i}].name = ${file.name}`);
      fileDetails.classList.remove("hidden");
      fileNameZone.innerHTML = file.name;
	  const fileSizeInMb = (file.size / 1024).toFixed(2);
      fileSizeZone.innerHTML = `${fileSizeInMb} Ko`;
	  console.log(fileSizeInMb);
    });
  }
});
