<script lang="ts">
	import boxImage from '../lib/assets/block/box.png';
	import coinImage from '../lib/assets/item/coin.png';
	import dirtImage from '../lib/assets/block/dirt.png';
	import grassImage from '../lib/assets/block/grass.png';
	import fruitImage from '../lib/assets/item/apple.png';
	import waterImage from '../lib/assets/block/water.png';
	import enemyImage from '../lib/assets/enemy/frog.png';

	import { BitmapEntryType } from './BitmapEntryType';

	export let entryType: BitmapEntryType;

	export let switchEntryType: () => void;

	let image: any = undefined;
	$: image = getBlockIcon(entryType);

	function getBlockIcon(entryType: BitmapEntryType) {
		switch (entryType) {
			case BitmapEntryType.NONE:
				return undefined;
			case BitmapEntryType.DIRT:
				return dirtImage;
			case BitmapEntryType.GRASS:
				return grassImage;
			case BitmapEntryType.WATER:
				return waterImage;
			case BitmapEntryType.BOX:
				return boxImage;
			case BitmapEntryType.COIN:
				return coinImage;
			case BitmapEntryType.FRUIT:
				return fruitImage;
			case BitmapEntryType.ENEMY:
				return enemyImage;
		}
	}
</script>

<!-- svelte-ignore a11y-click-events-have-key-events -->
<!-- svelte-ignore a11y-missing-attribute -->
<div on:click={switchEntryType} class="entryContainer">
	{#if image !== undefined}
		<img src={image} draggable="false" />
	{/if}
</div>

<style>
	.entryContainer {
		height: 1rem;
		width: 1rem;
		border: 1px solid black;
		user-select: none;
	}

	img {
		height: inherit;
		width: inherit;
	}
</style>
