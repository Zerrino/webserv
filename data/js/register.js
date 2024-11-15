class Register {
  constructor(form, fields, request) {
    this.form = form;
    this.fields = fields;
    this.request = request;
    this.validateonSubmit();
  }

  validateonSubmit() {
    this.form.addEventListener("submit", (e) => {
      e.preventDefault();
      var error = 0;
      this.fields.forEach((field) => {
        const input = document.querySelector(`#${field}`);
        if (this.validateFields(input) == false) {
          error++;
        }
      });
      if (error == 0) {
        this.form.submit();
      }
    });
  }

  validateFields(field) {
    if (field.type == "password") {
		if (field.value.trim() === "") {
			this.setStatus(
				field,
				`${field.name} should not be blank`,
				"error"
			  );
			  return false;
		}
      if (field.value.length < 8) {
        this.setStatus(
          field,
          `${field.name} must be at least 8 characters`,
          "error"
        );
        return false;
      } else {
        this.setStatus(field, null, "success");
        return true;
      }
    }
  }

  setStatus(field, message, status) {
    const errorMessage = field.parentElement.lastElementChild;

    if (status == "success") {
      if (errorMessage) {
        errorMessage.innerText = "";
      }
      field.classList.remove("input-error");
    }
    if (status == "error") {
      errorMessage.innerText = message;
      field.classList.add("input-error");
    }
  }
}


const createRequest = () => {
  const request = new XMLHttpRequest();
  const API_ENDPOINT = "http://localhost:80/data/ressources/database/"; // http://localhost:80/data/ressources/uploads/
  request.open("POST", API_ENDPOINT, true);
  request.onreadystatechange = () => {
    if (request.readyState === 4 && request.status === 200) {
      console.log(request.responseText);
    }
  };
  return request;
};

const form = document.getElementById("register-form");
if (form) {
  const fields = ["email", "password"];
  const request = createRequest();
  const validator = new Register(form, fields, request);
}
