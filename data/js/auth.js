class Auth {
  constructor() {
    document.querySelector("body").classList.add = "hidden";
    const auth = localStorage.getItem("auth");
    this.validateAuth(auth);
  }
  validateAuth(auth) {
    if (auth != 1) {
      window.location.replace("/");
    } else {
      document.querySelector("body").classList.remove = "hidden";
    }
  }
  logOut() {
    localStorage.removeItem("auth");
    window.location.replace("/");
  }
}
