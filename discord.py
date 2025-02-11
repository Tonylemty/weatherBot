import discord
from discord.ext import commands
import subprocess
import os
import json

# 設定機器人的指令前綴
intents = discord.Intents.default()
bot = commands.Bot(command_prefix="!", intents=intents)

# 用戶設定文件路徑
USER_SETTINGS = "user_settings.json"

# 加載用戶設定
def load_user_settings(user_id):
    if os.path.exists(USER_SETTINGS):
        with open(USER_SETTINGS, "r") as file:
            settings = json.load(file)
            return settings.get(str(user_id), {})
    return {}

# 保存用戶設定
def save_user_settings(user_id, settings):
    all_settings = {}
    if os.path.exists(USER_SETTINGS):
        with open(USER_SETTINGS, "r") as file:
            all_settings = json.load(file)
    all_settings[str(user_id)] = settings
    with open(USER_SETTINGS, "w") as file:
        json.dump(all_settings, file, indent=4)

# 當機器人啟動時執行
@bot.event
async def on_ready():
    print(f'Logged in as {bot.user}')

# 定義 !weather 指令
@bot.command(name="weather")
async def weather(ctx, city: str):
    user_id = ctx.author.id
    settings = load_user_settings(user_id)
    language = settings.get("language", "en")  # 預設語言為英文
    unit = settings.get("unit", "metric")      # 預設單位為攝氏

    # 檢查 C++ 可執行文件是否存在
    if not os.path.exists("./weather"):
        await ctx.send("Error: Weather executable not found.")
        return

    # 呼叫 C++ 可執行文件
    try:
        process = subprocess.Popen(["./weather", city, language, unit], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()

        # 檢查執行結果
        if process.returncode == 0:
            await ctx.send(f"```\n{stdout.decode()}\n```")
        else:
            await ctx.send(f"Error: {stderr.decode()}")
    except Exception as e:
        await ctx.send(f"An error occurred: {str(e)}")

# 定義 !history 指令
@bot.command(name="history")
async def history(ctx, num: str = "all"):
    user_id = ctx.author.id
    settings = load_user_settings(user_id)
    language = settings.get("language", "en")  # 預設語言為英文

    # 檢查 C++ 可執行文件是否存在
    if not os.path.exists("./weather"):
        await ctx.send("Error: Weather executable not found.")
        return

    # 呼叫 C++ 可執行文件
    try:
        process = subprocess.Popen(["./weather", "history", num, language], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()

        # 檢查執行結果
        if process.returncode == 0:
            await ctx.send(f"```\n{stdout.decode()}\n```")
        else:
            await ctx.send(f"Error: {stderr.decode()}")
    except Exception as e:
        await ctx.send(f"An error occurred: {str(e)}")

# 定義 !setlang 指令
@bot.command(name="setlang")
async def set_language(ctx, lang: str):
    user_id = ctx.author.id
    settings = load_user_settings(user_id)

    # 支援的語言列表
    supported_languages = ["en", "zh_tw", "zh_cn", "fr", "de", "ja", "kr", "es"]

    if lang in supported_languages:
        settings["language"] = lang
        save_user_settings(user_id, settings)
        await ctx.send(f"Language set to {lang}.")
    else:
        await ctx.send("Unsupported language. Supported languages: en, zh_tw, zh_cn, fr, de, ja, kr, es")

# 定義 !setunit 指令
@bot.command(name="setunit")
async def set_unit(ctx, unit: str):
    user_id = ctx.author.id
    settings = load_user_settings(user_id)

    # 支援的單位
    supported_units = ["metric", "imperial"]

    if unit in supported_units:
        settings["unit"] = unit
        save_user_settings(user_id, settings)
        await ctx.send(f"Unit set to {unit}.")
    else:
        await ctx.send("Unsupported unit. Supported units: metric, imperial")

# 運行機器人
bot.run("DISCORD_BOT_TOKEN")