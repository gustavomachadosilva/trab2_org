from pathlib import Path
import pandas as pd

EXCEL_NAME = 'sim_data.xlsx'

def main():
    folder = Path('./results')
    data = {
        'File': [],
        'sim_seconds': [],
        'sim_insts': [],
        'system.cpu.numCycles': [],
        'system.cpu.ipc': [],
        'host_seconds': [],
        'system.cpu.dcache.overall_miss_rate::total': [],
        'system.cpu.icache.overall_miss_rate::total': [],
        'system.cpu.dcache.overall_miss_rate::total': [],
        'system.l2cache.overall_misses::total': [],
        'system.l3cache.overall_miss_rate::total': [],
        'system.cpu.commit.branches': [],
        'system.cpu.branchPred.condPredicted': [],
        'system.cpu.branchPred.condIncorrect': []
    }

    data_2 = {
        'File': [],
        'sim_seconds': [],
        'system.cpu.ipc': [],
    }

    for file_path in folder.glob('*.txt'):
        if file_path.is_file():
            print(f"Reading file: {file_path.name}")
            find_statistics(file_path, data_2)
    
    create_excel(data_2)
            
            
def find_statistics(file_path, data):
    with open(file_path, 'r') as file:
        data['File'].append(file_path.name)
        for line in file:
            parts = line.split()
            if parts and (parts[0] in data.keys()):
                data[parts[0]].append(parts[1])


def create_excel(data):
    df = pd.DataFrame(data)

    df.to_excel(EXCEL_NAME, index=False)
    print("File Created!")
    

if __name__ == '__main__':
    main()