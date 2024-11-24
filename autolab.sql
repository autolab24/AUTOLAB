-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Servidor: localhost:3306
-- Tiempo de generación: 31-08-2024 a las 23:51:17
-- Versión del servidor: 10.4.32-MariaDB
-- Versión de PHP: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de datos: `autolab`
--

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `casilleros`
--

CREATE TABLE `casilleros` (
  `Casillero_id` int(2) NOT NULL,
  `Disponibilidad` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_spanish_ci;

--
-- Volcado de datos para la tabla `casilleros`
--

INSERT INTO `casilleros` (`Casillero_id`, `Disponibilidad`) VALUES
(1, 1),
(2, 0),
(3, 1),
(4, 1),
(5, 1),
(6, 1),
(7, 1),
(8, 1),
(9, 1),
(10, 1),
(11, 1),
(12, 1);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `elementos`
--

CREATE TABLE `elementos` (
  `EPC` varchar(40) NOT NULL,
  `Elemento` varchar(25) NOT NULL,
  `Codigo` int(7) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_spanish_ci;

--
-- Volcado de datos para la tabla `elementos`
--

INSERT INTO `elementos` (`EPC`, `Elemento`, `Codigo`) VALUES
('e2001757c1821710667e', 'Multimetro_1', 10001),
('EPC2', 'Multimetro_2', 10002),
('EPC3', 'Multimetro_3', 10003),
('EPC4', 'Multimetro_4', 10004),
('EPC5', 'Multimetro_5', 10005),
('EPC6', 'Multimetro_6', 10006),
('EPC7', 'Multimetro_7', 10007),
('EPC8', 'Multimetro_8', 10008),
('EPC9', 'Multimetro_9', 10009),
('EPC10', 'Multimetro_10', 100010),
('EPC11', 'Multimetro_11', 100011),
('EPC12', 'Multimetro_12', 100012),
('e2047725064269484110', 'Cables_1', 100013),
('EPC14', 'Cables_2', 100014),
('EPC15', 'Cables_3', 100015),
('EPC16', 'Cables_4', 100016),
('EPC17', 'Cables_5', 100017),
('EPC18', 'Cables_6', 100018),
('EPC19', 'Cables_7', 100019),
('EPC20', 'Cables_8', 100020),
('EPC21', 'Cables_9', 100021),
('EPC22', 'Cables_10', 100022),
('EPC23', 'Cables_11', 100023),
('EPC24', 'Cables_12', 100024),
('30833b2ddd91400000', 'Protoboard_1', 100025),
('EPC26', 'Protoboard_2', 100026),
('EPC27', 'Protoboard_3', 100027),
('EPC28', 'Protoboard_4', 100028),
('EPC29', 'Protoboard_5', 100029),
('EPC30', 'Protoboard_6', 100030),
('EPC31', 'Protoboard_7', 100031),
('EPC32', 'Protoboard_8', 100032),
('EPC33', 'Protoboard_9', 100033),
('EPC34', 'Protoboard_10', 100034),
('EPC35', 'Protoboard_11', 100035),
('EPC36', 'Protoboard_12', 100036),
('e2047725064269484110', 'Destornilladores_1', 100037),
('EPC38', 'Destornilladores_2', 100038),
('EPC39', 'Destornilladores_3', 100039),
('EPC40', 'Destornilladores_4', 100040),
('EPC41', 'Destornilladores_5', 100041),
('EPC42', 'Destornilladores_6', 100042),
('EPC43', 'Destornilladores_7', 100043),
('EPC44', 'Destornilladores_8', 100044),
('EPC45', 'Destornilladores_9', 100045),
('EPC46', 'Destornilladores_10', 100046),
('EPC47', 'Destornilladores_11', 100047),
('EPC48', 'Destornilladores_12', 100048),
('303acb340d6685ac688c295', 'Kit_Pinzas_1', 100049),
('EPC50', 'Kit_Pinzas_2', 100050),
('EPC51', 'Kit_Pinzas_3', 100051),
('EPC52', 'Kit_Pinzas_4', 100052),
('EPC53', 'Kit_Pinzas_5', 100053),
('EPC54', 'Kit_Pinzas_6', 100054),
('EPC55', 'Kit_Pinzas_7', 100055),
('EPC56', 'Kit_Pinzas_8', 100056),
('EPC57', 'Kit_Pinzas_9', 100057),
('EPC58', 'Kit_Pinzas_10', 100058),
('EPC59', 'Kit_Pinzas_11', 100059),
('EPC60', 'Kit_Pinzas_12', 100060);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `historial_prestamos`
--

CREATE TABLE `historial_prestamos` (
  `Nombre_Usuario` varchar(40) NOT NULL,
  `Identificacion` varchar(12) NOT NULL,
  `Casillero_id` int(12) NOT NULL,
  `Elementos` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_spanish_ci;

--
-- Volcado de datos para la tabla `historial_prestamos`
--

INSERT INTO `historial_prestamos` (`Nombre_Usuario`, `Identificacion`, `Casillero_id`, `Elementos`) VALUES
('Cristiano Ronaldo', '7777777777', 2, 'Multimetro_2,Multimetro_1');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `usuarios`
--

CREATE TABLE `usuarios` (
  `Nombre` varchar(40) NOT NULL,
  `Codigo_RFID` varchar(40) NOT NULL,
  `Identificacion` varchar(12) NOT NULL,
  `Correo` varchar(40) NOT NULL,
  `Programa` varchar(15) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_spanish_ci;

--
-- Volcado de datos para la tabla `usuarios`
--

INSERT INTO `usuarios` (`Nombre`, `Codigo_RFID`, `Identificacion`, `Correo`, `Programa`) VALUES
('Mario Alejandro Perez Jaramillo', 'EAD9D00C', '1006515674', '240182003@javerianacali.edu.co', 'Electronica'),
('Carlos Andres Giraldo castaneda', 'E33889FB', '1225484789', 'cgiraldoc@javerianacali.edu.co', 'Electronica'),
('Tamura onichan', 'C3C29CFB', '255647481', 'naruto@gmail.com', 'Electronica'),
('Juanes Real G x life', 'C2D8AE80', '777777', 'eyey@gmail.com', 'Electrónica'),
('Cristiano Ronaldo', 'A39B84FB', '7777777777', 'elbichosiuu@alnassar.com', 'Electronica');

--
-- Índices para tablas volcadas
--

--
-- Indices de la tabla `casilleros`
--
ALTER TABLE `casilleros`
  ADD PRIMARY KEY (`Casillero_id`);

--
-- Indices de la tabla `elementos`
--
ALTER TABLE `elementos`
  ADD PRIMARY KEY (`Codigo`);

--
-- Indices de la tabla `historial_prestamos`
--
ALTER TABLE `historial_prestamos`
  ADD PRIMARY KEY (`Identificacion`);

--
-- Indices de la tabla `usuarios`
--
ALTER TABLE `usuarios`
  ADD PRIMARY KEY (`Identificacion`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
