<script lang="ts">
	import { BitmapEntryType, createEmptyBitmapData } from '$lib/BitmapEntryType';
	import BitmapEntry from '../lib/BitmapEntry.svelte';

	let blockSize = 0.08;
	let width = 4;
	let height = 2;

	let data: BitmapEntryType[][] = createEmptyBitmapData(width, height, blockSize);

	function getNextEntryType(entryType: BitmapEntryType): BitmapEntryType {
		const values = Object.values(BitmapEntryType).filter((v) => typeof v === 'number') as number[];
		const currentIndex = values.indexOf(entryType);
		const nextIndex = (currentIndex + 1) % values.length;
		return values[nextIndex] as BitmapEntryType;
	}

	let textareaValue = '';

	function detectTextareaValueChange(textareaValue: string) {
		data = textareaValue.split('\n').map((r) => r.split('').map((v) => Number.parseInt(v)));
	}
	function detectDataChange(data: BitmapEntryType[][]) {
		if (data.length == 0) {
			data = createEmptyBitmapData(width, height, blockSize);
		}
		textareaValue = data.join('\n').replaceAll(',', '');
	}

	$: detectTextareaValueChange(textareaValue);
	$: detectDataChange(data);
</script>

<div class="main">
	<div>
		<div>
			<label>width:</label>
			<input bind:value={width} />
		</div>
		<div>
			<label>height:</label>
			<input bind:value={height} />
		</div>
		<div>
			<label>blocksize:</label>
			<input bind:value={blockSize} />
		</div>
		<button on:click={() => (data = createEmptyBitmapData(width, height, blockSize))}
			>Recreate!</button
		>
	</div>

	<div class="bitmap">
		{#each data as row, rowNumber (rowNumber)}
			<div class="column">
				{#each row as currentEntry, columnNumber (columnNumber)}
					<BitmapEntry
						entryType={currentEntry}
						switchEntryType={() => (currentEntry = getNextEntryType(currentEntry))}
					/>
				{/each}
			</div>
		{/each}
	</div>

	<textarea rows={height / blockSize} bind:value={textareaValue} />
</div>

<style>
	.main {
		display: flex;
		flex-direction: column;
		gap: 1rem;
	}
	.bitmap {
		display: flex;
		flex-direction: column;
		overflow: scroll;
		width: fit-content;
		height: fit-content;
	}
	.column {
		display: flex;
		flex-direction: row;
	}
</style>
