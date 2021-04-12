
<?php 
        $title = "Главная страница";
        include "header.php";

?>

        <h1 class="mt-5" style="color:red;">Привет!!</h1>

<?php
        if (isset($_POST['button_ON']))
        {
                //exec("/home/pi/my_linux_projects/projects/recent_projects/gtk2_glade3");
                echo "button ON is set ";
        }
        if (isset($_POST['button_OFF']))
                echo "button OFF is set ";
?>

        <form method="post"> 
                <input type="submit" class="mt-5" name="button_ON" value="ON"/><br>
                <input type="submit" class="mt-5" name="button_OFF" value="OFF"/>
        </form>

        <h1 class="mt-5">Главная страница</h1>
        <h1 class="mt-5"></h1>

<?php
        //exec("emacs");
        //system("emacs");

        // while(1)
        // {
        //         echo date('Y-m-d H:i:s');
        //         sleep(100);
        // }

        // echo file_get_contents('/home/pi/Documents/esp32data.txt');

        echo date('Y-m-d H:i:s');

        // echo ''.'<br>'';
        //echo phpinfo();

        print_r($_POST);

        include "footer.php";
?>
