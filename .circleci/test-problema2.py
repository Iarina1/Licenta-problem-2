def check_problema2(problema2, string):
    try:
        with open(problema2, 'r') as file:
            for line in file:
                line_without_spaces = line.replace(" ", "")
                if string in line_without_spaces:
                    return True
        return False
    except IOError:
        print(f"Error: Could not open file '{file_path}'")


problema2 = 'problema2.ino'
string1 = 'led_pin=5'
string2 =  'digitalWrite(led_pin,HIGH)'

if check_problema2(problema2, string1):
    print("Test 1 passed")
    if check_problema2(problema2, string2):
      print("Test 2 passed")
      print("Punctaj total: 10")
    else:
      print("Test 2 failed")
      print("Punctaj total: 5")
else:
    print("Test 1 failed")
    print("Punctaj total: 0")
