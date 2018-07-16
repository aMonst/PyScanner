import requests

def getInformation():
    return "测试脚本", 1

def startScan(url, method):
    if(method == "GET"):
	    response = requests.get(url)
    else:
        response = requests.post(url)
	
    if response.status_code == 200:
        return True, url
    else:
        return False