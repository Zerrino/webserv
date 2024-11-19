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
      if (this.validateFields() == false) {
        return;
      } else {
        const formData = new FormData(this.form);
        const params = Object.fromEntries(formData.entries());
		console.log(JSON.stringify(params));
        this.request.send(JSON.stringify(params));
        this.showModal();
      }
    });
  }

  validateFields() {
    const password = document.getElementById("password");
    const repeated = document.getElementById("cpassword");

    if (password.value.trim() === "") {
      this.setStatus(password, "Password field should not be blank", "error");
      return false;
    } else if (password.value.length < 8) {
      this.setStatus(
        password,
        "Password must be at least 8 characters",
        "error"
      );
      return false;
    } else this.setStatus(password, "", "success");
    return this.checkPasswordEquality(password, repeated);
  }

  checkPasswordEquality(password, repeated) {
    if (password.value != repeated.value) {
      this.setStatus(repeated, "Passwords are not corresponding", "error");
      return false;
    } else {
      this.setStatus(password, "", "success");
      return true;
    }
  }

  setStatus(field, message, status) {
    const errorMessageDiv = field.parentElement.lastElementChild;

    if (status == "success") {
      if (errorMessageDiv) {
        errorMessageDiv.innerText = "";
      }
      field.classList.remove("input-error");
    }
    if (status == "error") {
      errorMessageDiv.innerText = message;
      field.classList.add("input-error");
    }
  }

  showModal() {
    const modal = document.getElementById("register-modal");
    const btn = modal.getElementsByTagName("button")[0];
    btn.addEventListener("click", (e) => {
      e.preventDefault();
      window.location.replace("/");
    });
    modal.classList.remove("hidden");
  }
}

const createRequest = () => {
  const request = new XMLHttpRequest();
  const API_ENDPOINT = "/data/ressources/database/";
  request.open("POST", API_ENDPOINT, true);
  request.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
  request.onreadystatechange = () => {
    if (request.readyState === 4 && request.status === 200) {
      console.log(request.responseText);
    }
  };
  return request;
};

const form = document.getElementById("register-form");
if (form) {
  const fields = ["email", "password", "cpassword"];
  const request = createRequest();
  const validator = new Register(form, fields, request);
}
