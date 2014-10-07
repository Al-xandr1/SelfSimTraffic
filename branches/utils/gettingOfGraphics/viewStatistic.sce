PATH = 'C:\Users\Alexander\University\Telecommunications\WORK_read_only\gettingOfGraphics\statistics\';
TRAFFIC_TYPE = "traffic.xml";
PROFILE_TYPE = "profile.xml";
SEPARATOR = "_";
COUNT_ELEMENT_IN_NAME = 3;//3 части в названии файла

BYTESPERSEC_INDEX = 1;        TRAFFIC_MEAN_INDEX = 1;
TRAFFIC_VARIANCE_INDEX = 2;   LIFETIME_MEAN_INDEX = 2;
HURST_INDEX = 3;              SOURCE_BORN_RATE_INDEX = 3;

//Функция считывающая необходимые скалярные величины из файла типа *_traffic.xml
function [result] = getSimplePoint_traffic(fileName)
    documentTraffic = xmlRead(PATH + fileName);
    result(1) = getNumber(documentTraffic, "BYTESPERSEC");//take element from <BYTESPERSEC> ...</>
    result(2) = getNumber(documentTraffic, "TRAFFIC-VARIANCE"); //take element from <TRAFFIC-VARIANCE> ...</>
    result(3) = getNumber(documentTraffic, "HURST");//take element from <HURST> ...</>
endfunction

//Функция считывающая необходимые скалярные величины из файла типа *_profile.xml
function [result] = getSimplePoint_profile(fileName)
    documentProfile = xmlRead(PATH + fileName);
    result(1) = getNumber(documentProfile, "TRAFFIC-MEAN");//take element from <TRAFFIC-MEAN> ...</> //эта величина в псевдопакетах!!!
    result(2) = getNumber(documentProfile, "LIFETIME-MEAN"); //take element from <LIFETIME-MEAN> ...</> 
    result(3) = getNumber(documentProfile, "SOURCE-BORN-RATE");//take element from <SOURCE-BORN-RATE> ...</>
endfunction

//Вырезает нужное сечение в матрице точек
//              index = 1, 2, 3
//                      1 - "BYTESPERSEC" or "TRAFFIC-MEAN"
//                      2 - "TRAFFIC-VARIANCE" or "LIFETIME-MEAN"
//                      3 - HURST" or "SOURCE-BORN-RATE"
function [y] = getSingleVector(complexVector, index)
    n = size(complexVector, 1);
    y = complexVector(1:n, index);
endfunction

//Получение данных из всех файлов. fileNames список (вектор-столбец) файлов
function [trafficPoints, trafficTimes, profilePoints, profileTimes] = processAllFiles(fileNames)
    fileCount = size(fileNames, 1);
    trafficPoints = [];
    trafficTimes = [];
    profilePoints = [];
    profileTimes = [];
   
    if (fileCount < 1) then
        error(msprintf("processAllFiles: нет файлов для обработки"));
    end
    
    for i = 1 : fileCount
        fileName = fileNames(i);
          
        elements = strsplit(fileName, SEPARATOR, COUNT_ELEMENT_IN_NAME);//3 части в названии файла
        typeOfFile = elements(3);
    
        if (strcmpi(typeOfFile, TRAFFIC_TYPE) == 0) then 
            //[BYTESPERSEC   TRAFFIC-VARIANCE   HURST]
            trafficPoint = getSimplePoint_traffic(fileName)';
            
            trafficPoints = addToArray(trafficPoints, trafficPoint);
            trafficTimes = addToArray(trafficTimes, elements(1));
            
        elseif (strcmpi(typeOfFile, PROFILE_TYPE) == 0) then
            //[TRAFFIC-MEAN   LIFETIME-MEAN   SOURCE-BORN-RATE]
            profilePoint = getSimplePoint_profile(fileName)';
            
            profilePoints = addToArray(profilePoints, profilePoint);
            profileTimes = addToArray(profileTimes, elements(1));
            
        else
            error(msprintf("processAllFiles: неизвестный тип файла"));
        end
    end
endfunction
    

function viewStatistic(folder)
    PATH = PATH + folder + '\';
    xmlFiles = getAppropriateFiles("*.xml");
    xmlFiles = invert(xmlFiles);
    printf("Список фалов: "); disp(xmlFiles);
    printf("\n");    
    
    [trafficPoints, trafficTimes, profilePoints, profileTimes] = processAllFiles(xmlFiles)
    
    printf("\n");    
    printf("Матрица точек для файлов _Traffic: "); disp(trafficPoints);
    printf("Массив лет: "); disp(trafficTimes);
    printf("\n");    
    printf("Матрица точек для файлов _Profile: "); disp(profilePoints);
    printf("Массив лет: "); disp(profileTimes); 

    
    bytePerSecGRAPHIC = getSingleVector(trafficPoints, BYTESPERSEC_INDEX)';// ' - для получения вектор строки
    trafficVarianceGRAPHIC = getSingleVector(trafficPoints, TRAFFIC_VARIANCE_INDEX)';// ' - для получения вектор строки
    hurstGRAPHIC = getSingleVector(trafficPoints, HURST_INDEX)';// ' - для получения вектор строки
    
    trafficMeanGRAPHIC = getSingleVector(trafficPoints, TRAFFIC_MEAN_INDEX)';// ' - для получения вектор строки
    liftTimeMeanGRAPHIC = getSingleVector(trafficPoints, LIFETIME_MEAN_INDEX)';// ' - для получения вектор строки
    sourceBornRateGRAPHIC = getSingleVector(trafficPoints, SOURCE_BORN_RATE_INDEX)';// ' - для получения вектор строки

    scf();//0
    plot2d(getIndexes(trafficTimes)', bytePerSecGRAPHIC,        [1], leg = "bytePerSec"); 
    scf();//1
    plot2d(getIndexes(trafficTimes)', trafficVarianceGRAPHIC,   [2], leg = "trafficVarianceGRAPHIC");
    scf();//2
    plot2d(getIndexes(trafficTimes)', hurstGRAPHIC,             [3], leg = "hurstGRAPHIC"); 
    scf();//3
    plot2d(getIndexes(profileTimes)', trafficMeanGRAPHIC,       [4], leg = "trafficMeanGRAPHIC");
    scf();//4
    plot2d(getIndexes(profileTimes)', liftTimeMeanGRAPHIC,      [5], leg = "liftTimeMeanGRAPHIC"); 
    scf();//5
    plot2d(getIndexes(profileTimes)', sourceBornRateGRAPHIC,    [6], leg = "sourceBornRateGRAPHIC");
endfunction








//-------------------------------HELPER METHODS---------------------------------
//Получение числа их тега с именем _string из xml документа document
function [Number] = getNumber(document, _string)
    xmlList = xmlXPath(document, "//" + _string + "/text()");//take element from <%_string%> ...</>
    Number = strtod(xmlList(1).content);// string parsing
endfunction

//Получение списка файлов, соответствующих шаблону из директории по умолчанию 
function [fileNames] = getAppropriateFiles(pattern)
   cd(PATH);
   fileNames = ls(pattern);
endfunction

function [result] = addToArray(array, item)
    result = [array ; item]    
endfunction

function [y] = getIndexes(x)
    n = size(x, 'r');
    y = [];
    for i = 1 : n
        y(i) = i;
    end
endfunction

//Инвертируем массив-столбец
function [invX] = invert(x)
    n = size(x, 'r');
    invX = [];
    for (i = 1 : n )
        invX = addToArray(invX, x(n - i + 1));
    end
endfunction

