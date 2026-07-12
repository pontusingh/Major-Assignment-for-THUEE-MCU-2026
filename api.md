请求格式:
```cpp
const char* weather_url = "https://restapi.amap.com/v3/weather/weatherInfo?key=xxxxxx&city=110108&extensions=base&output=JSON";
HTTPClient http;
http.begin(weather_url);
```

我的api key:
ef86e2b18420d4017baf74d9b3d77c2b

返回json:
```markdown
| 字段名       | 含义                     | 备注                                      |
|--------------|--------------------------|-------------------------------------------|
| status       | 返回状态                 | 值为0或1；1：成功；0：失败               |
| count        | 返回结果总数             |                                           |
| info         | 返回的状态信息           |                                           |
| infocode     | 返回状态说明             | 10000代表正确                             |
| lives        | 实况天气数据信息         |                                           |
| province     | 省份名                   |                                           |
| city         | 城市名                   |                                           |
| adcode       | 区域编码                 |                                           |
| weather      | 天气现象（汉字描述）     |                                           |
| temperature  | 实时气温                 | 单位：摄氏度                              |
| winddirection| 风向描述                 |                                           |
| windpower    | 风力级别                 | 单位：级                                  |
| humidity     | 空气湿度                 |                                           |
| reporttime   | 数据发布的时间           |                                           |
```