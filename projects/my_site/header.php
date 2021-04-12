
<!DOCTYPE html>
<html lang="en">
	<head>
			<meta charset="UTF-8">
			<meta http-equiv="X-UA-Compatible" content="IE=edge">
			<!-- <meta http-equiv="refresh" content="2"> -->
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
			<script type="text/javascript" src="jquery.js"></script>
			<title><?=$title?></title>
			<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
	</head>

	<body>
	<div class="container mt-5">
			<header>
					<a href="index.php">Главная</a> |
					<a href="about.php">Справка</a>
			</header>

			<h1 class="mt-5"></h1>

			<div id="content"></div>

				<script>
					function show()
					{
						$.ajax({
								url: "esp32data.txt",
								cache: false,
								success: function(html){
										$("#content").html(html);
								}
						});
					}
				
					$(document).ready(function(){
						show();
						setInterval('show()',1000);
					});
				</script>

                