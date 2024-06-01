 
//Parametros do Firebase
var firebaseConfig = {
apiKey: "AIzaSyBkrZ9svvXWv8VmZ3b3ajkLQSwIpog_MpY",
authDomain: "controlebombas.firebaseapp.com",
databaseURL: "https://controlebombas-default-rtdb.firebaseio.com",
projectId: "controlebombas",
storageBucket: "controlebombas.appspot.com",
messagingSenderId: "823504727067",
appId: "1:823504727067:web:8547cf28729c27c1f1dcbe"
};


//Inicializando o Firebase
firebase.initializeApp(firebaseConfig); 

//Variáveis de Consulta dos dados do usuários no Firebase para o login.
var login_db;
var senha_db;

//Consultando Usuário no Firebase e salvando na variável login_db
var ref_login = firebase.database().ref("login");
ref_login.on("value", function(snapshot) {login_db = snapshot.val();})

//Consultando senha no Firebase e salvando a variável senha_db
var ref_senha = firebase.database().ref("senha");                    
ref_senha.on("value", function(snapshot) {senha_db = snapshot.val();})

    //Função para entrar: Ela compara os dados de login do DB com os apresentados pelo usuário assim que o botão "Entrar" é clicado.
    function logar(){

        //Variáveis que recebem os dados de login e senha escritas pelo usuário
        var login = document.getElementById('login').value;
        var senha = document.getElementById('senha').value;

            //Comparando o login e senha escritos pelo usuário com os do banco de dados para fazer o login ou não.
            if(login == login_db && senha == senha_db){
                alert('Seja Bem-Vindo!');
                location.href = "home.html";
            }else{
                alert('Usuario e/ou senha incorretos');
            }

    }
