<?php
$servername = "172.20.10.5";
$username = "root";  // Usuario predeterminado de XAMPP
$password = "";      // Contraseña predeterminada (puede estar vacía)
$dbname = "autolab";

// Crear la conexión
$conn = new mysqli($servername, $username, $password, $dbname);

// Verificar la conexión
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>

<?php
// Nueva función: Consultar Elemento por EPC
if ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['accion']) && $_GET['accion'] == 'consultar_elemento') {
    $epc = $_GET['epc'];

    $sql = "SELECT Elemento FROM Elementos WHERE EPC='$epc'";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo $row['Elemento'];
    } else {
        echo "Elemento no encontrado";
    }
}
?>

<?php
//Actualizar disponibilidad de los Casilleros

if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['accion']) && $_POST['accion'] == 'actualizar_disponibilidad') {
    $casillero_id = $_POST['casillero_id'];
    $disponibilidad = $_POST['disponibilidad'];  // Se espera que 'disponibilidad' sea '1' o '0'

    $sql = "UPDATE Casilleros SET Disponibilidad = '$disponibilidad' WHERE Casillero_id = '$casillero_id'";

    if ($conn->query($sql) === TRUE) {
        echo json_encode(['success' => 'Disponibilidad actualizada correctamente']);
    } else {
        echo json_encode(['error' => 'Error actualizando disponibilidad: ' . $conn->error]);
    }
}

//$conn->close();
?>


<?php
//Consultar un usuario por codigo RFID

if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['accion']) && $_POST['accion'] == 'consultar_usuario') {
    $codigo_rfid = $_POST['codigo_rfid'];

    $sql = "SELECT * FROM Usuarios WHERE Codigo_RFID = '$codigo_rfid'";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        // Convertimos el resultado en un array asociativo
        $usuario = $result->fetch_assoc();
        echo json_encode($usuario);
    } else {
        echo json_encode(['error' => 'Usuario no encontrado']);
    }
}

?>

<?php
//Consultar Disponibilidad de Casilleros
if ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['accion']) && $_GET['accion'] == 'consultar_disponibilidad') {
    $casillero_id = $_GET['casillero_id'];

    $sql = "SELECT Disponibilidad FROM Casilleros WHERE Casillero_id='$casillero_id'";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo $row['Disponibilidad'];
        //echo "Disponibilidad: " . $row['Disponibilidad'];
    } else {
        echo "Casillero no encontrado";
    }
}
?>

<?php
//Registrar un nuevo usuario
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['accion']) && $_POST['accion'] == 'registrar_usuario') {
    $nombre = $_POST['nombre'];
    $codigo_rfid = $_POST['codigo_rfid'];
    $identificacion = $_POST['identificacion'];
    $correo = $_POST['correo'];
    $programa = $_POST['programa'];

    // Verificar si el usuario ya existe
    //$sql_check = "SELECT * FROM Usuarios WHERE Codigo_RFID='$codigo_rfid'";
    //$result = $conn->query($sql_check);

    //if ($result->num_rows > 0) {
      //  echo "Error: El usuario con esta identificación ya existe.";
    //} else {
        // Si no existe, registrarlo
        $sql = "INSERT INTO Usuarios (Nombre, Codigo_RFID, Identificacion, Correo, Programa)
                VALUES ('$nombre', '$codigo_rfid', '$identificacion', '$correo', '$programa')";

        if ($conn->query($sql) === TRUE) {
            echo "Usuario registrado con éxito";
        } else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    //}
}
?>


<?php
//Registrar un prestamo
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['accion']) && $_POST['accion'] == 'registrar_prestamo') {
    $nombre_usuario = $_POST['nombre_usuario'];
    $identificacion = $_POST['identificacion'];
    $casillero_id = $_POST['casillero_id'];
    $elementos = $_POST['elementos']; // Debería ser un string de elementos separados por comas

    $sql = "INSERT INTO historial_prestamos (Nombre_Usuario, Identificacion, Casillero_id, Elementos)
            VALUES ('$nombre_usuario', '$identificacion', '$casillero_id', '$elementos')";

    if ($conn->query($sql) === TRUE) {
        echo "Préstamo registrado con éxito";

        // Actualizar disponibilidad del casillero
        $sql_update = "UPDATE Casilleros SET Disponibilidad='No' WHERE Casillero_id='$casillero_id'";
        $conn->query($sql_update);
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
}
?>

<?php
//Consultar Historial de Préstamos
if ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['accion']) && $_GET['accion'] == 'consultar_historial') {
    $identificacion = $_GET['identificacion'];

    $sql = "SELECT * FROM historial_prestamos WHERE Identificacion='$identificacion'";
    $result = $conn->query($sql);

    if ($result->num_rows > 0) {
        $prestamo = $result->fetch_assoc();
        echo json_encode($prestamo);
    } else {
        echo "Sin prestamos";
    }
}
?>


<?php
if ($_SERVER['REQUEST_METHOD'] == 'DELETE' && isset($_GET['accion']) && $_GET['accion'] == 'eliminar_prestamo') {
    $identificacion = $_GET['identificacion'];

    $sql = "DELETE FROM historial_prestamos WHERE Identificacion='$identificacion'";
    if ($conn->query($sql) === TRUE) {
        echo json_encode(['success' => 'Préstamo eliminado correctamente']);
    } else {
        echo json_encode(['error' => 'Error al eliminar préstamo: ' . $conn->error]);
    }
}
?>
