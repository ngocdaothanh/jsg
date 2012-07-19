<?php
    // 引数の処理
    if ($argc == 2)
    {
        $filename = $argv[1];
        $hasTitleRow = false;
    }
    else if ($argc == 3 && $argv[1] === '-t') {
        $filename = $argv[2];
        $hasTitleRow = true;
    }
    else {
        echo "Usage: php makeplist.php [-t] <filename>\n";
        echo "  -t             The file has a title row.\n";
        exit();
    }
    
    
    // ファイルオープン
    if (!file_exists($filename)) {
        echo "No such file $filename\n";
        exit();
    }
    
    $pos  = strrpos($filename, '.');
    $filename_out = substr($filename, 0, $pos).'.plist';
    
    $fp_in = fopen($filename, "r");
    $fp_out = fopen($filename_out, "w");
    
    // タイトル行を取得
    if ($hasTitleRow) {
        $data = fgetcsv($fp_in, 1000, ",");
        $num = count($data);
        $keys = array();
        for ($i = 0; $i < $num; $i++) {
            $keys[$i] = $data[$i];
        }
    }
    
    // plist生成
    fwrite($fp_out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fwrite($fp_out, "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n");
    fwrite($fp_out, "<plist version=\"1.0\">\n");
    fwrite($fp_out, "<array>\n");

while ($data = fgetcsv($fp_in, 1000, ",")) {
    $num = count($data);
    
    fwrite($fp_out, "\t<dict>\n");
    for ($i = 0; $i < $num; $i++) {
        if ($hasTitleRow) {
            fwrite($fp_out, "\t\t<key>$keys[$i]</key>\n");
        }
        else {
            fwrite($fp_out, "\t\t<key>keyname$i</key>\n");
        }
        fwrite($fp_out, "\t\t<string>".$data[$i]."</string>\n");
    }
    fwrite($fp_out, "\t</dict>\n");
}
fclose($fp_in);

fwrite($fp_out, "</array>\n");
fwrite($fp_out, "</plist>\n");

fclose($fp_out);

echo "Generated:$filename_out\n";
