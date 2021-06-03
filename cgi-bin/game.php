<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>결과!</title>
    <style>
      * {
        font-size: 16px;
        font-family: Consolas, sans-serif;
      }
    </style>
<script>
function goBack() {
window.history.back();
}
</script>
</head>
<body>
<?php
//$color = $_POST['number'];
// $sport = $_POST['sport'];
fscanf(STDIN, "number=%d\n", $number); // reads number from STDIN
//echo $number; echo ": 입력값\n";
// $i = 0; //i변수에 1을 대입합니다.
// while($i < $number) //i가 10보다 작거나 같을 때 반복합니다
// {
// 	echo $i.": 출력\n"; //i를 출력합니다.
// 	$i++; //i를 1씩 증가합니다.(증감식)
// }
if ($number == 0)
{
	echo "숫자를 제대로 입력해주세요";
}
else if ($number > 42)
{
	echo "DOWN!";
}
else if ($number < 42)
{
	echo "UP!";
}
else
{
	echo "정답입니다";
}
?>
		<button onclick="goBack()">Go Back</button>
</body>
</html>