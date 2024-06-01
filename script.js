

//--------------------------------Configurações do Firebase-------------------------//
var firebaseConfig = {
  apiKey: "AIzaSyBkrZ9svvXWv8VmZ3b3ajkLQSwIpog_MpY",
  authDomain: "controlebombas.firebaseapp.com",
  databaseURL: "https://controlebombas-default-rtdb.firebaseio.com",
  projectId: "controlebombas",
  storageBucket: "controlebombas.appspot.com",
  messagingSenderId: "823504727067",
  appId: "1:823504727067:web:8547cf28729c27c1f1dcbe"
};

firebase.initializeApp(firebaseConfig);

//---------------CONTROLE DE MODO AUTOMATICO E MANUAL--------------//

// Quando o botão AUTOMÁTICO for clicado for clicado
$("#modoauto").click(function() {
  // Obtenha uma referência para a chave no Firebase que você deseja atualizar
  var ref = firebase.database().ref("modo");
  // Atualize o valor da chave no Firebase
  ref.set(true);
});

// Quando o botão MANUAL for clicado
$("#modomanual").click(function() {
  
  var resposta = confirm("Deseja realmente alterar para MODO MANUAL?")
    if(resposta ==true){
  // Obtenha uma referência para a chave no Firebase que você deseja atualizar
  var ref = firebase.database().ref("modo");
  // Atualize o valor da chave no Firebase
  ref.set(false);
    }


});

// --------------------- Monitoramento do Status do Modo----------------------------------//

// Obtenha uma referência para a chave no Firebase que você deseja monitorar
var modo = firebase.database().ref("modo");

// Monitore as atualizações em tempo real da chave 
modo.on("value", function(snapshot) {
  // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
  var modo = snapshot.val();
    if(modo==true){
    $("#statusmodo").text("Automático").removeClass("status-desligada").addClass("status-ligada");
    
    }
    if(modo==false){
    $("#statusmodo").text("Manual").removeClass("status-ligada").addClass("status-desligada");
    
    }
});


//----------------------------------CONTROLE DA BOMBA 1-------------------------------------//

// Quando o botão Ligar da Bomba1 for clicado
$("#ligb1").click(function() {

  var stslg1 = firebase.database().ref("modo");

  // Monitore as atualizações em tempo real da chave 
  stslg1.once("value", function(snapshot) {
    // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
    var stslg1 = snapshot.val();
    if(stslg1 == false){
      var stslg1 = firebase.database().ref("bomba1");
      
  stslg1.set(true);
    }
    else{
      alert("Ative o MODO MANUAL para alterar as configurações");
    }
    
  });
});


// Quando o botão desligar da Bomba1 for clicado
$("#desb1").click(function() {
  // Obtenha uma referência para a chave no Firebase que você deseja atualizar

  var stsdlg1 = firebase.database().ref("modo");

  // Monitore as atualizações em tempo real da chave 
  stsdlg1.once("value", function(snapshot) {
    // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
    var stsdlg1 = snapshot.val();
    if(stsdlg1 == false){
      var stsdlg1 = firebase.database().ref("bomba1");
      
  stsdlg1.set(false);
    }
    else{
      alert("Ative o MODO MANUAL para alterar as configurações");
    }
    
  });
});


//---------------Monitoramento do Status da Bomba 1 - Ligada ou desligada--------------------//

// Obtenha uma referência para a chave no Firebase que você deseja monitorar
var statusb1 = firebase.database().ref("bomba1");

// Monitore as atualizações em tempo real da chave 
statusb1.on("value", function(snapshot) {
  
  // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
  var valorstatusb1 = snapshot.val();
    if(valorstatusb1==true){
    $("#statusb1").text("Ligada").removeClass("status-desligada").addClass("status-ligada");
    }
    if(valorstatusb1==false){
    $("#statusb1").text("Desligada").removeClass("status-ligada").addClass("status-desligada");
    
    }
});





//----------------------------------CONTROLE DA BOMBA 2------------------------------------//

// Quando o botão Ligar da Bomba2 for clicado
$("#ligb2").click(function() {
  var stslg2 = firebase.database().ref("modo");

  // Monitore as atualizações em tempo real da chave 
  stslg2.once("value", function(snapshot) {
    // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
    var stslg2 = snapshot.val();
    if(stslg2 == false){
      var stslg2 = firebase.database().ref("bomba2");
      
  stslg2.set(true);
    }
    else{
      alert("Ative o MODO MANUAL para alterar as configurações");
    }
    
  });
});

// Quando o botão desligar da Bomba1 for clicado
$("#desb2").click(function() {
  var stsdlg2 = firebase.database().ref("modo");

  // Monitore as atualizações em tempo real da chave 
  stsdlg2.once("value", function(snapshot) {
    // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
    var stsdlg2 = snapshot.val();
    if(stsdlg2 == false){
      var stsdlg2 = firebase.database().ref("bomba2");
      
  stsdlg2.set(false);
    }
    else{
      alert("Ative o MODO MANUAL para alterar as configurações");
    }
    
  });
});

// --------------- Monitoramento do Status da Bomba 2 - Ligada ou desligada ----------------//

// Obtenha uma referência para a chave no Firebase que você deseja monitorar
var statusb2 = firebase.database().ref("bomba2");

// Monitore as atualizações em tempo real da chave 
statusb2.on("value", function(snapshot) {
  // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
  var valorstatusb2 = snapshot.val();
    if(valorstatusb2==true){
    $("#statusb2").text("Ligada").removeClass("status-desligada").addClass("status-ligada");
    
    }
    if(valorstatusb2==false){
    $("#statusb2").text("Desligada").removeClass("status-ligada").addClass("status-desligada");
    
    }
});



//-------------------------------MONITORAMENTO DO NIVEL DO TANQUE---------------------------//

// Obtenha uma referência para a chave no Firebase que você deseja monitorar
var nivel = firebase.database().ref("nivel");

// Monitore as atualizações em tempo real da chave 
nivel.on("value", function(snapshot) {
  // Quando a chave for atualizada no Firebase, atualize o valor no seu elemento HTML
  var valor = snapshot.val();
    $("#valor-nivel").text(valor +"%");
    $(".barrap").css("width", valor + "%");
  
});
