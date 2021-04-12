
<?php 
    $title = "Информация";
    require "header.php";

        if (isset($_POST['ON']))
        {
                exec("/home/pi/my_linux_projects/projects/recent_projects/gtk2_glade3");
        }

        echo date('Y-m-d H:i:s');
?>

                <h1>Справка</h1>

<?php 
    require "footer.php";
?>
