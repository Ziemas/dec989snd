import json

with open("989snd_d.json", "r") as file:
    data = json.load(file)
    
    for func in data['functions']:
        addr = func['address']
        name = func['name']
        size = func['size']
        print(f'{name} = 0x{addr:x}; // type:func size:0x{size:x}')


    data_addr = 0
    for s in data['sections']:
        if 'name' in s:
            if s['name'] == '.data':
                data_addr = s['address']
                break

    for g in data['global_variables']:
        name = g['name']
        address = g['address'] + data_addr
        storage_loc = g['storage']['location']
        if storage_loc == "data":
            if 'size' in g:
                size = g['size']
                print(f'{name} = 0x{address:x}; // size:0x{size:x}')
            else:
                print(f'{name} = 0x{address:x};')
        


