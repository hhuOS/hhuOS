export enum BitmapEntryType {
	NONE = 0,
	DIRT = 1,
	GRASS = 2,
	WATER = 3,
	BOX = 4,
	COIN = 5,
	FRUIT = 6,
	ENEMY = 7,
}

export function createEmptyBitmapData(width: number, height: number, blockSize: number) {
	let rows = Math.floor(height / blockSize);
	let columns = Math.floor(width / blockSize);

	const data = [];
	for (let i = 0; i < rows; i++) {
		const row: BitmapEntryType[] = [];
		for (let j = 0; j < columns; j++) {
			row.push(BitmapEntryType.NONE);
		}
		data.push(row);
	}
	return data;
}
